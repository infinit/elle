#pragma once

#include <elle/With.hh>
#include <elle/cryptography/random.hh>

#include <elle/reactor/Scope.hh>
#include <elle/reactor/for-each.hh>
#include <elle/reactor/scheduler.hh>

namespace elle
{
  namespace athena
  {
    namespace paxos
    {
      /*-----.
      | Peer |
      `-----*/

      template <typename T, typename Version, typename ClientId>
      Client<T, Version, ClientId>::Peer::Peer(ClientId id)
        : _id(id)
      {}

      template <typename T, typename Version, typename ClientId>
      void
      Client<T, Version, ClientId>::Peer::print(std::ostream& output) const
      {
        elle::fprintf(output, "%f(%f)", elle::type_info(*this), this->id());
      }

      /*-------------.
      | Construction |
      `-------------*/

      template <typename T, typename Version, typename ClientId>
      Client<T, Version, ClientId>::Client(ClientId id, Peers peers)
        : _id(id)
        , _peers(std::move(peers))
        , _conflict_backoff(true)
        , _round(0)
      {
        ELLE_ASSERT(!this->_peers.empty());
      }

      template <typename T, typename Version, typename ClientId>
      void
      Client<T, Version, ClientId>::peers(Peers peers)
      {
        this->_peers = std::move(peers);
      }

      class Unavailable
        : public elle::Error
      {
      public:
        Unavailable()
          : elle::Error("peer unavailable")
        {}

        Unavailable(elle::serialization::SerializerIn& input)
          : elle::Error(input)
        {}
      };

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

      template <typename T, typename Version, typename ClientId>
      boost::optional<typename Client<T, Version, ClientId>::Accepted>
      Client<T, Version, ClientId>::choose(elle::Option<T, Quorum> const& value)
      {
        return this->choose(Version(), std::move(value));
      }

      template <typename T, typename Version, typename ClientId>
      bool
      Client<T, Version, ClientId>::_check_headcount(Quorum const& q,
                                                     int reached,
                                                     bool reading,
                                                     bool raise) const
      {
        ELLE_LOG_COMPONENT("athena.paxos.Client");
        ELLE_DEBUG("reached %s peers", reached);
        auto size = signed(q.size());
        if (reached <= (size - (reading ? 1 : 0)) / 2)
        {
          if (raise)
          {
            ELLE_TRACE("too few peers to reach consensus: {} of {}",
                       reached, size);
            throw TooFewPeers(reached, size);
          }
          else
            return false;
        }
        else
          return true;
      }

      template <typename T, typename Version, typename ClientId>
      boost::optional<typename Client<T, Version, ClientId>::Accepted>
      Client<T, Version, ClientId>::choose(
        elle::_detail::attribute_r_t<Version> version,
        elle::Option<T, Quorum> const& value)
      {
        ELLE_LOG_COMPONENT("athena.paxos.Client");
        ELLE_TRACE_SCOPE("%s: choose %f", *this, value);
        int backoff = 1;
        Quorum q;
        for (auto const& peer: this->_peers)
          q.insert(peer->id());
        ELLE_DUMP("quorum: %s", q);
        boost::optional<Accepted> previous;
        while (true)
        {
          ++this->_round;
          Proposal proposal(std::move(version), this->_round, this->_id);
          ELLE_DEBUG("%s: send proposal: %s", *this, proposal)
          {
            int reached = 0;
            elle::reactor::for_each_parallel(
              this->_peers,
              [&] (std::unique_ptr<Peer> const& peer) -> void
              {
                try
                {
                  ELLE_DEBUG_SCOPE("%s: send proposal %s to %s",
                                   *this, proposal, peer);
                  if (auto p = peer->propose(q, proposal))
                  {
                    if (!previous || previous->proposal < p->proposal)
                    {
                      // FIXME: what if previous was accepted and p is not ?
                      ELLE_DEBUG_SCOPE("%s: value already accepted at %f: %f",
                                       *this, p->proposal, p->value);
                      previous = std::move(p);
                    }
                  }
                  ++reached;
                }
                catch (Unavailable const& e)
                {
                  ELLE_TRACE("%s: peer %s unavailable: %s",
                             *this, peer, e.what());
                }
              },
              std::string("send proposal"));
            if (previous && previous->confirmed)
              return previous;
            ELLE_TRACE("check headcount")
              this->_check_headcount(q, reached);
            if (previous)
            {
              ELLE_DEBUG("replace value with %s", previous->value);
              if (proposal < previous->proposal)
              {
                version = previous->proposal.version;
                this->_round = previous->proposal.round;
                ELLE_DEBUG("retry at version %s round %s", version, this->_round);
                continue;
              }
            }
          }
          ELLE_DEBUG("%s: send acceptation", *this)
          {
            int reached = 0;
            bool conflicted = false;
            elle::reactor::for_each_parallel(
              this->_peers,
              [&] (std::unique_ptr<Peer> const& peer) -> void
              {
                try
                {
                  ELLE_DEBUG_SCOPE("%s: send acceptation %s to %s",
                                   *this, proposal, *peer);
                  auto minimum = peer->accept(
                    q, proposal, previous ? previous->value : value);
                  // FIXME: If the majority doesn't conflict, the value was
                  // still chosen - right ? Take that in account.
                  if (proposal < minimum)
                  {
                    ELLE_DEBUG("%s: conflicted proposal on peer %s: %s",
                               *this, peer, minimum);
                    version = minimum.version;
                    this->_round = minimum.round;
                    conflicted = true;
                    elle::reactor::break_parallel();
                  }
                  ++reached;
                }
                catch (Unavailable const& e)
                {
                  ELLE_TRACE("%s: peer %s unavailable: %s",
                             *this, peer, e.what());
                }
              },
              std::string("send acceptation"));
            if (conflicted)
            {
              auto rn = elle::cryptography::random::generate<uint8_t>(1, 8);
              auto delay = 100_ms * rn * backoff;
              if (this->_conflict_backoff)
              {
                ELLE_TRACE("%s: conflicted proposal, retry in %s", this, delay);
                elle::reactor::sleep(delay);
              }
              else
                ELLE_TRACE("%s: conflicted proposal, retry", this);
              backoff = std::min(backoff * 2, 64);
              continue;
            }
            else
              this->_check_headcount(q, reached);
          }
          ELLE_TRACE("%s: chose %f", this, previous ? previous->value : value);
          ELLE_DEBUG("%s: send confirmation", *this)
          {
            auto reached = 0;
            elle::reactor::for_each_parallel(
              this->_peers,
              [&] (std::unique_ptr<Peer> const& peer) -> void
              {
                try
                {
                  ELLE_DEBUG_SCOPE("%s: send confirmation %s to %s",
                                   *this, proposal, *peer);
                  peer->confirm(q, proposal);
                  ++reached;
                }
                catch (Unavailable const& e)
                {
                  ELLE_TRACE("%s: peer %s unavailable: %s",
                             *this, peer, e.what());
                }
              },
              std::string("send confirmation"));
            this->_check_headcount(q, reached);
          }
          break;
        }
        return previous;
      }

      template <typename T, typename Version, typename ClientId>
      boost::optional<T>
      Client<T, Version, ClientId>::get()
      {
        return this->state().value;
      }

      template <typename T, typename Version, typename CId>
      typename Client<T, Version, CId>::State
      Client<T, Version, CId>::state()
      {
        ELLE_LOG_COMPONENT("athena.paxos.Client");
        ELLE_TRACE_SCOPE("%s: get value", *this);
        Quorum q;
        for (auto const& peer: this->_peers)
          q.insert(peer->id());
        ELLE_DUMP("quorum: %s", q);
        auto reached = 0;
        boost::optional<typename Client::Accepted> res;
        boost::optional<typename Server::WrongQuorum> wrong_quorum;
        elle::reactor::for_each_parallel(
          this->_peers,
          [&] (std::unique_ptr<Peer> const& peer) -> void
          {
            try
            {
              ELLE_DEBUG_SCOPE("%s: get from %s", *this, *peer);
              try
              {
                auto accepted = peer->get(q);
                if (accepted)
                {
                  if (!res || res->proposal < accepted->proposal)
                  {
                    ELLE_DEBUG("accept proposal %f", accepted->proposal);
                    res.emplace(std::move(accepted.get()));
                  }
                  else
                    ELLE_DEBUG("skip proposal %f", accepted->proposal);
                }
              }
              catch (typename Server::WrongQuorum const& e)
              {
                if (!e.proposal())
                {
                  // Not handled pre 0.4.0.
                  ELLE_WARN("throwing wrong quorum error unconditionally "
                            "because elle version is < 0.4.");
                  throw;
                }
                if (!wrong_quorum || *wrong_quorum->proposal() < *e.proposal())
                {
                  ELLE_DEBUG("accept wrong quorum %f", *e.proposal());
                  wrong_quorum.emplace(e);
                }
                else
                  ELLE_DEBUG("skip wrong quorum %f", *e.proposal());
              }
              ++reached;
            }
            catch (Unavailable const& e)
            {
              ELLE_TRACE("%s: peer %s unavailable: %s", *this, peer, e.what());
            }
          },
          std::string("get quorum"));
        this->_check_headcount(q, reached, true);
        if (wrong_quorum &&
            (!res || res->proposal < wrong_quorum->proposal()))
        {
          ELLE_DEBUG("throw %f", wrong_quorum);
          throw *wrong_quorum;
        }
        else if (res)
          return State(res->value.template get<T>(), q, res->proposal);
        else
          return State(boost::none, q, boost::none);
      }

      /*----------.
      | Printable |
      `----------*/

      template <typename T, typename Version, typename CId>
      void
      Client<T, Version, CId>::print(std::ostream& output) const
      {
        elle::fprintf(output, "paxos::Client(%f)", this->_id);
      }
    }
  }
}
