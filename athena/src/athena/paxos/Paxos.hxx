#ifndef ATHENA_PAXOS_PAXOS_HXX
# define ATHENA_PAXOS_PAXOS_HXX

namespace athena
{
  namespace paxos
  {
    /*------.
    | Types |
    `------*/

    template <typename T, typename ServerId>
    bool
    Paxos<T, ServerId>::Proposal::operator <(Proposal const& rhs) const
    {
      if (this->round < rhs.round)
        return true;
      return this->sender < rhs.sender;
    }

    /*-------------.
    | Construction |
    `-------------*/

    template <typename T, typename ServerId>
    Paxos<T, ServerId>::Paxos(ServerId id, Peers peers)
      : _id(std::move(id))
      , _peers(std::move(peers))
      , _round(0)
      , _accepted()
      , _has_accepted()
      , _chosen(false)
    {}

    template <typename T, typename ServerId>
    T
    Paxos<T, ServerId>::choose(T const& value_)
    {
      T value = value_; // FIXME: no need to copy if not replaced
      ELLE_LOG_COMPONENT("athena.paxos.Paxos");
      ELLE_TRACE_SCOPE("%s: choose %s", *this, value);
      if (!this->_chosen)
      {
        ++this->_round;
        {
          auto previous = this->propose(this->_id, this->_round);
          int reached = 0;
          for (auto& peer: this->_peers) // FIXME: parallel
          {
            try
            {
              if (auto p = peer->propose(this->_id, this->_round))
                if (!previous || previous->proposal < p->proposal)
                {
                  ELLE_DEBUG_SCOPE("%s: value already accepted: %s",
                                   *this, p.get());
                  previous = p;
                }
              ++reached;
            }
            catch (typename Peer::Unavailable const&)
            {}
          }
          if (reached < this->_peers.size() / 2)
            throw elle::Error("too few peers are available to reach consensus");
          if (previous)
          {
            ELLE_DEBUG("%s: replace value with %s", *this, previous->value);
            value = std::move(previous->value);
          }
        }
        {
          this->accept(this->_id, this->_round, value);
          int reached = 0;
          for (auto& peer: this->_peers) // FIXME: parallel
          {
            try
            {
              peer->accept(this->_id, this->_round, value);
              ++reached;
            }
            catch (typename Peer::Unavailable const&)
            {}
          }
          if (reached < this->_peers.size() / 2)
            throw elle::Error("to few peers are available to reach consensus");
        }
        this->_chosen = true;
      }
      ELLE_TRACE("%s: chose %s", *this, this->_accepted->value);
      return this->_accepted->value;
    }

    template <typename T, typename ServerId>
    T
    Paxos<T, ServerId>::choose()
    {
      ELLE_LOG_COMPONENT("athena.paxos.Paxos");
      ELLE_TRACE_SCOPE("%s: choose", *this);
      reactor::wait(this->_has_accepted);
      if (!this->_chosen)
        return this->choose(this->_accepted->value);
      else
        return this->_accepted->value;
    }

    template <typename T, typename ServerId>
    boost::optional<typename Paxos<T, ServerId>::Accepted>
    Paxos<T, ServerId>::propose(ServerId const& sender, int round)
    {
      Proposal p{round, sender};
      if (!this->_minimum || this->_minimum < p)
        this->_minimum = p;
      return this->_accepted;
    }

    template <typename T, typename ServerId>
    void
    Paxos<T, ServerId>::accept(ServerId const& sender, int round,
                               T const& value)
    {
      ELLE_LOG_COMPONENT("athena.paxos.Paxos");
      ELLE_TRACE_SCOPE("%s: accept %s at round %s from %s",
                       *this, value, round, sender);
      if (!this->_accepted)
        this->_accepted.emplace();
      this->_accepted->proposal.sender = sender;
      this->_accepted->proposal.round = round;
      this->_accepted->value = value;
      this->_has_accepted.open();
    }

    /*----------.
    | Printable |
    `----------*/

    template <typename T, typename ServerId>
    void
    Paxos<T, ServerId>::print(std::ostream& output) const
    {
      elle::fprintf(output, "Paxos(%s)", this->_id);
    }

    /*------------.
    | Unavailable |
    `------------*/

    template <typename T, typename ServerId>
    Paxos<T, ServerId>::Peer::Unavailable::Unavailable()
      : elle::Error("paxos peer unavailable")
    {}
  }
}

#endif
