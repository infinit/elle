#ifndef ATHENA_PAXOS_CLIENT_HXX
# define ATHENA_PAXOS_CLIENT_HXX

# include <elle/With.hh>

# include <reactor/Scope.hh>
# include <reactor/scheduler.hh>

namespace athena
{
  namespace paxos
  {
    /*-------------.
    | Construction |
    `-------------*/

    template <typename T, typename ClientId>
    Client<T, ClientId>::Client(ClientId id, Peers peers)
      : _id(id)
      , _peers(std::move(peers))
      , _round(0)
    {}

    class TooFewPeers
      : public elle::Error
    {
    public:
      TooFewPeers(int effective, int total)
        : elle::Error(
          elle::sprintf(
            "too few peers are available to reach consensus: %s of %s",
            effective, total))
      {}

      TooFewPeers(elle::serialization::SerializerIn& input)
        : elle::Error(input)
      {}
    };

    /*----------.
    | Consensus |
    `----------*/

    template <typename T, typename ClientId>
    boost::optional<T>
    Client<T, ClientId>::choose(T const& value_)
    {
      T const* value = &value_;
      boost::optional<T> new_value;
      ELLE_LOG_COMPONENT("athena.paxos.Client");
      ELLE_TRACE_SCOPE("%s: choose %s", *this, printer(*value));
      while (true)
      {
        ++this->_round;
        Proposal proposal{this->_round, this->_id};
        ELLE_DEBUG("%s: send proposal: %s", *this, proposal)
        {
          boost::optional<Accepted> previous;
          int reached = 0;
          elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
          {
            for (auto& peer: this->_peers)
            {
              scope.run_background(
                elle::sprintf("%s: paxos proposal",
                              reactor::scheduler().current()->name()),
                [&]
                {
                  try
                  {
                    if (auto p = peer->propose(proposal))
                      if (!previous || previous->proposal < p->proposal)
                      {
                        ELLE_DEBUG_SCOPE("%s: value already accepted at %s: %s",
                                         *this, p->proposal, printer(p->value));
                        previous = std::move(p);
                      }
                    ++reached;
                  }
                  catch (typename Peer::Unavailable const& e)
                  {
                    ELLE_TRACE("%s: peer %s unavailable: %s",
                               *this, peer, e.what());
                  }
                });
            }
            reactor::wait(scope);
          };
          if (reached <= this->_peers.size() / 2)
          {
            ELLE_TRACE("%s: too few peers to reach consensus", *this);
            throw TooFewPeers(reached, this->_peers.size());
          }
          if (previous)
          {
            ELLE_DEBUG("%s: replace value with %s",
                       *this, printer(previous->value));
            new_value.emplace(std::move(previous->value));
            value = &*new_value;
          }
        }
        ELLE_DEBUG("%s: send acceptation", *this)
        {
          int reached = 0;
          bool conflicted = false;
          elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
          {
            for (auto& peer: this->_peers)
            {
              scope.run_background(
                elle::sprintf("paxos proposal"),
                [&]
                {
                  try
                  {
                    auto minimum = peer->accept(proposal, *value);
                    // FIXME: If the majority doesn't conflict, the value was
                    // still chosen - right ? Take that in account.
                    if (proposal < minimum)
                    {
                      ELLE_DEBUG("%s: conflicted proposal on peer %s: %s",
                                 *this, peer, minimum);
                      conflicted = true;
                      scope.terminate_now();
                    }
                    ++reached;
                  }
                  catch (typename Peer::Unavailable const& e)
                  {
                    ELLE_TRACE("%s: peer %s unavailable: %s",
                               *this, peer, e.what());
                  }
                });
            }
            reactor::wait(scope);
          };
          if (conflicted)
          {
            // FIXME: we could (should) potentially skip rounds to catch up to
            // the latest - right ?
            // FIXME: random wait to avoid livelock
            ELLE_DEBUG("%s: conflicted proposal, retry", *this);
            continue;
          }
          else
            if (reached <= this->_peers.size() / 2)
            {
              ELLE_TRACE("%s: too few peers to reach consensus", *this);
              throw TooFewPeers(reached, this->_peers.size());
            }
            else
              break;
        }
      }
      ELLE_TRACE("%s: chose %s", *this, printer(*value));
      return new_value;
    }

    /*----------.
    | Printable |
    `----------*/

    template <typename T, typename ClientId>
    void
    Client<T, ClientId>::print(std::ostream& output) const
    {
      elle::fprintf(output, "paxos::Client(%f)", this->_id);
    }

    /*------------.
    | Unavailable |
    `------------*/

    template <typename T, typename ClientId>
    Client<T, ClientId>::Peer::Unavailable::Unavailable()
      : elle::Error("paxos peer unavailable")
    {}
  }
}

#endif
