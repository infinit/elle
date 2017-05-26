#pragma once

#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index_container.hpp>

#include <elle/With.hh>
#include <elle/serialization/Serializer.hh>

#include <elle/reactor/Scope.hh>

namespace elle
{
  namespace athena
  {
    namespace paxos
    {
      /*---------.
      | Proposal |
      `---------*/

      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      Server<T, Version, ClientId, ServerId>::Proposal::Proposal(
        Version version_, int round_, ClientId sender_)
        : version(version_)
        , round(round_)
        , sender(sender_)
      {}

      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      Server<T, Version, ClientId, ServerId>::Proposal::Proposal()
        : version()
        , round()
        , sender()
      {}

      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      bool
      Server<T, Version, ClientId, ServerId>::Proposal::operator ==(
        Proposal const& rhs) const
      {
        return std::tie(this->version, this->round, this->sender)
          == std::tie(rhs.version, rhs.round, rhs.sender);
      }

      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      bool
      Server<T, Version, ClientId, ServerId>::Proposal::operator <(
        Proposal const& rhs) const
      {
        return std::tie(this->version, this->round, this->sender)
          < std::tie(rhs.version, rhs.round, rhs.sender);
      }

      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      void
      Server<T, Version, ClientId, ServerId>::Proposal::serialize(
        elle::serialization::Serializer& s)
      {
        s.serialize("version", this->version);
        s.serialize("round", this->round);
        s.serialize("sender", this->sender);
      }

      /*---------.
      | Accepted |
      `---------*/

      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      Server<T, Version, ClientId, ServerId>::Accepted::Accepted(
        Proposal proposal_,
        elle::Option<T, Quorum> value_,
        bool confirmed_)
        : proposal(std::move(proposal_))
        , value(std::move(value_))
        , confirmed(confirmed_)
      {}

      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      Server<T, Version, ClientId, ServerId>::Accepted::Accepted(
        elle::serialization::SerializerIn& s, elle::Version const& v)
        : proposal()
        , value(Quorum())
        , confirmed(false)
      {
        this->serialize(s, v);
      }

      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      void
      Server<T, Version, ClientId, ServerId>::Accepted::serialize(
        elle::serialization::Serializer& serializer,
        elle::Version const& version)
      {
        serializer.serialize("proposal", this->proposal);
        if (version >= elle::Version(0, 1, 0))
        {
          serializer.serialize("value", this->value);
          serializer.serialize("confirmed", this->confirmed);
        }
        else if (serializer.out())
        {
          auto& s
            = static_cast<elle::serialization::SerializerOut&>(serializer);
          if (this->value.template is<T>())
            s.serialize("value", this->value.template get<T>());
          else
            ELLE_ABORT("Athena cannot serialize quorum changes pre 0.1.0");
        }
        else
        {
          auto& s = static_cast<elle::serialization::SerializerIn&>(serializer);
          this->value.template emplace<T>(s.deserialize<T>("value"));
          this->confirmed = true;
        }
      }

      /*------------.
      | WrongQuorum |
      `------------*/

      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      Server<T, Version, ClientId, ServerId>::WrongQuorum::WrongQuorum(
        Quorum expected,
        Quorum effective,
        boost::optional<Proposal> proposal)
        : elle::Error(
          elle::print("wrong quorum: %f instead of %f", effective, expected))
        , _expected(std::move(expected))
        , _effective(std::move(effective))
        , _proposal(std::move(proposal))
      {}

      template <typename T, typename Version, typename CId, typename SId>
      Server<T, Version, CId, SId>::WrongQuorum::WrongQuorum(
        elle::serialization::SerializerIn& input, elle::Version const& v)
        : Super(input)
      {
        this->_serialize(input, v);
      }

      template <typename T, typename Version, typename CId, typename SId>
      void
      Server<T, Version, CId, SId>::WrongQuorum::serialize(
        elle::serialization::Serializer& s, elle::Version const& version)
      {
        Super::serialize(s, version);
        this->_serialize(s, version);
      }

      template <typename T, typename Version, typename CId, typename SId>
      void
      Server<T, Version, CId, SId>::WrongQuorum::_serialize(
        elle::serialization::Serializer& s, elle::Version const& version)
      {
        s.serialize("expected", this->_expected);
        s.serialize("effective", this->_effective);
        if (version >= elle::Version(0, 4, 0))
          s.serialize("proposal", this->_proposal);
        else if (version < elle::Version(0, 1, 0))
        {
          auto dummy = Version();
          s.serialize("version", dummy);
        }
      }

      // We used to name this "athena::...".  For backward
      // compatibility, remove the "elle:" prefix we recently
      // introduced.
      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      const elle::serialization::Hierarchy<elle::Exception>::Register<
        typename Server<T, Version, ClientId, ServerId>::WrongQuorum>
      Server<T, Version, ClientId, ServerId>::
      _register_wrong_quorum_serialization{
        boost::replace_all_copy(
            elle::serialization::Hierarchy<elle::Exception>::Register<
        typename Server<T, Version, ClientId, ServerId>::WrongQuorum>
            ::default_name(),
                                "elle::athena", "athena")
          };

      /*-------------.
      | PartialState |
      `-------------*/

      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      Server<T, Version, ClientId, ServerId>::PartialState::PartialState(
        Proposal p)
        : elle::Error(elle::sprintf("partial state: %s", p))
        , _proposal(std::move(p))
      {}

      template <typename T, typename Version, typename CId, typename SId>
      Server<T, Version, CId, SId>::PartialState::PartialState(
        elle::serialization::SerializerIn& input, elle::Version const& v)
        : Super(input)
      {
        this->_serialize(input, v);
      }

      template <typename T, typename Version, typename CId, typename SId>
      void
      Server<T, Version, CId, SId>::PartialState::serialize(
        elle::serialization::Serializer& s, elle::Version const& version)
      {
        Super::serialize(s, version);
        this->_serialize(s, version);
      }

      template <typename T, typename Version, typename CId, typename SId>
      void
      Server<T, Version, CId, SId>::PartialState::_serialize(
        elle::serialization::Serializer& s, elle::Version const& version)
      {
        s.serialize("proposal", this->_proposal);
      }

      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      const elle::serialization::Hierarchy<elle::Exception>::Register<
        typename Server<T, Version, ClientId, ServerId>::PartialState>
      Server<T, Version, ClientId, ServerId>::
        _register_partial_state_serialization{
        boost::replace_all_copy(
            elle::serialization::Hierarchy<elle::Exception>::Register<
        typename Server<T, Version, ClientId, ServerId>::PartialState>
            ::default_name(),
                                "elle::athena", "athena")
          };

      /*-------------.
      | Construction |
      `-------------*/

      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      Server<T, Version, ClientId, ServerId>::Server(
        ServerId id, Quorum quorum, elle::Version version)
        : _id(std::move(id))
        , _quorum(quorum)
        , _value()
        , _version(version)
        , _partial(false)
        , _state()
      {
        ELLE_ASSERT_CONTAINS(this->_quorum, this->_id);
        this->_register_wrong_quorum_serialization.poke();
        this->_register_partial_state_serialization.poke();
      }

      /*--------.
      | Details |
      `--------*/

      template <typename T, typename Version, typename CId, typename SId>
      struct Server<T, Version, CId, SId>::_Details
      {
        static
        void
        check_quorum(Server<T, Version, CId, SId>& self,
                     Quorum q,
                     bool get = false)
        {
          ELLE_LOG_COMPONENT("athena.paxos.Server");
          auto expected = self._quorum;
          if (get &&
              self._state &&
              self._state->accepted &&
              self._state->accepted->confirmed &&
              self._state->accepted->value.template is<Quorum>())
            expected = self._state->accepted->value.template get<Quorum>();
          if (q != expected)
          {
            ELLE_TRACE("quorum is wrong: %f instead of %f", q, expected);
            auto current = self.current_value();
            throw WrongQuorum(
              std::move(expected), std::move(q),
              current ? current->proposal : boost::optional<Proposal>());
          }
        }

        /// Check we don't skip any version and the previous version was
        /// confirmed before starting a new one.
        static
        bool
        check_confirmed(Server<T, Version, CId, SId>& self, Proposal const& p)
        {
          ELLE_LOG_COMPONENT("athena.paxos.Server");
          if (self.version() < elle::Version(0, 1, 0))
          {
            ELLE_DUMP_SCOPE("elle version %s is too old for confirmation",
                            self.version());
            return true;
          }
          if (!self._state)
          {
            ELLE_DUMP("no confirmation needed as there never was a proposal");
            return true;
          }
          auto const& version = self._state->proposal.version;
          ELLE_DUMP_SCOPE("confirm current paxos version %s", version);
          if (version >= p.version)
          {
            ELLE_DUMP("proposed version %s is not more recent", p.version);
            return true;
          }
          if (version == p.version - 1 &&
              self._state->accepted &&
              self._state->accepted->confirmed)
          {
            ELLE_DUMP("ready to be commited");
            return true;
          }
          ELLE_DUMP("unconfirmed");
          return false;
        }
      };

      /*----------.
      | Consensus |
      `----------*/

      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      boost::optional<typename Server<T, Version, ClientId, ServerId>::Accepted>
      Server<T, Version, ClientId, ServerId>::propose(Quorum q, Proposal p)
      {
        ELLE_LOG_COMPONENT("athena.paxos.Server");
        ELLE_TRACE_SCOPE("%s: get proposal: %s ", *this, p);
        if (this->_state &&
            this->_state->accepted &&
            this->_state->accepted->proposal.version > p.version)
        {
          ELLE_DEBUG(
            "refuse proposal for version %s in favor of version %s",
            p.version, this->_state->accepted->proposal.version);
          return this->_state->accepted;
        }
        if (_Details::check_confirmed(*this, p))
        {
          if (this->_state && p.version > this->_state->proposal.version)
          {
            auto& accepted = this->_state->accepted;
            ELLE_ASSERT(accepted);
            if (accepted->value.template is<T>())
            {
              ELLE_DEBUG("commit previous value");
              this->_value.emplace(std::move(accepted->value.template get<T>()));
            }
            else
            {
              ELLE_DEBUG("commit previous quorum election");
              this->_quorum =
                std::move(accepted->value.template get<Quorum>());
            }
            this->_state.reset();
          }
          _Details::check_quorum(*this, q);
        }
        else
        {
          ELLE_DEBUG("acknowledge partial state");
          this->_partial = true;
          this->_state.reset();
        }
        if (!this->_state)
        {
          ELLE_DEBUG("accept first proposal for version %s", p.version);
          this->_state.emplace(std::move(p));
          return {};
        }
        else
        {
          if (this->_state->proposal < p)
          {
            ELLE_DEBUG("update minimum proposal for version %s", p.version);
            this->_state->proposal = std::move(p);
          }
          return this->_state->accepted;
        }
      }

      template <typename T, typename Version, typename CId, typename SId>
      typename Server<T, Version, CId, SId>::Proposal
      Server<T, Version, CId, SId>::accept(
        Quorum q, Proposal p, elle::Option<T, Quorum> value)
      {
        ELLE_LOG_COMPONENT("athena.paxos.Server");
        ELLE_TRACE_SCOPE("%s: accept for %f: %f", *this, p, value);
        if (!this->_partial)
          _Details::check_quorum(*this, q);
        if (!this->_state || this->_state->proposal < p)
        {
          ELLE_WARN("%s: someone malicious sent an accept before propose",
                    this);
          elle::err("propose before accepting");
        }
        if (p < this->_state->proposal)
        {
          ELLE_TRACE("discard obsolete accept, current proposal is %s",
                     this->_state->proposal);
          return this->_state->proposal;
        }
        auto& version = *this->_state;
        if (!(p < version.proposal))
        {
          if (!version.accepted)
            version.accepted.emplace(std::move(p), std::move(value), false);
          else
          {
            // FIXME: assert !confirmed || new_value == value ?
            version.accepted->proposal = std::move(p);
            version.accepted->value = std::move(value);
          }
        }
        return version.proposal;
      }

      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      void
      Server<T, Version, ClientId, ServerId>::confirm(Quorum q, Proposal p)
      {
        ELLE_LOG_COMPONENT("athena.paxos.Server");
        ELLE_TRACE_SCOPE("%s: confirm proposal %s", *this, p);
        if (!this->_partial)
          _Details::check_quorum(*this, q);
        if (this->_state && p.version < this->_state->proposal.version)
        {
          ELLE_TRACE("discard obsolete confirm, current proposal is %s",
                     this->_state->proposal);
          return;
        }
        if (!this->_state ||
            this->_state->proposal < p ||
            !this->_state->accepted)
        {
          ELLE_WARN("%s: someone malicious sent a confirm before propose/accept",
                    this);
          elle::err("propose and accept before confirming");
        }
        auto& accepted = *this->_state->accepted;
        if (!accepted.confirmed)
        {
          accepted.confirmed = true;
          if (this->_partial)
          {
            this->_quorum = q;
            this->_partial = false;
          }
        }
      }

      template <typename T, typename Version, typename CId, typename SId>
      typename Server<T, Version, CId, SId>::Quorum
      Server<T, Version, CId, SId>::current_quorum() const
      {
        if (this->_state &&
            this->_state->accepted &&
            this->_state->accepted->confirmed &&
            this->_state->accepted->value.template is<Quorum>())
          return this->_state->accepted->value.template get<Quorum>();
        else
          return this->_quorum;
      }

      template <typename T, typename Version, typename CId, typename SId>
      boost::optional<typename Server<T, Version, CId, SId>::Accepted>
      Server<T, Version, CId, SId>::current_value() const
      {
        if (!this->_state)
          return {};
        else if (this->_state->accepted && this->_state->accepted->confirmed)
          if (this->_state->accepted->value.template is<T>())
            return this->_state->accepted;
          else
            return Accepted(this->_state->proposal, *this->_value, true);
        else if (this->_value)
        {
          // This is slightly hackish: in case the current proposal is not
          // confirmed, the value is committed for the previous version, and we
          // use a null round number and node id because nobody probably
          // cares. The other solution would be to also commit the latest
          // proposal, but I don't think it's worth the trouble, especially
          // given it would require a serialization version change.
          return Accepted(Proposal(this->_state->proposal.version - 1, {}, {}),
                          *this->_value, true);
        }
        else
          return {};
      }

      template <typename T, typename Version, typename CId, typename SId>
      Version
      Server<T, Version, CId, SId>::current_version() const
      {
        if (this->_state)
        {
          if (this->_state->accepted &&
              this->_state->accepted->confirmed)
            return this->_state->version();
          else
            return this->_state->version() - 1;
        }
        else
          return Version();
      }

      template <typename T, typename Version, typename CId, typename SId>
      boost::optional<typename Server<T, Version, CId, SId>::Accepted>
      Server<T, Version, CId, SId>::get(Quorum q)
      {
        ELLE_LOG_COMPONENT("athena.paxos.Server");
        ELLE_TRACE_SCOPE("%s: get", *this);
        _Details::check_quorum(*this, q, true);
        return this->current_value();
      }

      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      Server<T, Version, ClientId, ServerId>::VersionState::VersionState(
        Proposal p, boost::optional<Accepted> a)
        : proposal(std::move(p))
        , accepted(std::move(a))
      {}

      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      Server<T, Version, ClientId, ServerId>::VersionState::VersionState(
        elle::serialization::SerializerIn& s, elle::Version const& v)
      {
        this->serialize(s, v);
      }

      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      void
      Server<T, Version, ClientId, ServerId>::VersionState::serialize(
        elle::serialization::Serializer& s, elle::Version const& v)
      {
        s.serialize("proposal", this->proposal);
        s.serialize("accepted", this->accepted);
      }

      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      Version
      Server<T, Version, ClientId, ServerId>::VersionState::version() const
      {
        return this->proposal.version;
      }

      /*--------------.
      | Serialization |
      `--------------*/

      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      Server<T, Version, ClientId, ServerId>::Server(
        elle::serialization::SerializerIn& s, elle::Version const& v)
        : _id()
        , _quorum()
        , _value()
        , _version()
        , _partial(false)
        , _state()
      {
        this->serialize(s, v);
      }

      // FIXME: use splitted serialization
      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      void
      Server<T, Version, ClientId, ServerId>::serialize(
        elle::serialization::Serializer& s, elle::Version const& v)
      {
        s.serialize_context<elle::Version>(this->_version);
        s.serialize("id", this->_id);
        s.serialize("quorum", this->_quorum);
        if (v >= elle::Version(0, 1, 0))
          s.serialize("value", this->_value);
        using VersionsState = boost::multi_index::multi_index_container<
          VersionState,
          boost::multi_index::indexed_by<
            boost::multi_index::ordered_unique<
              boost::multi_index::const_mem_fun<
                VersionState, Version, &VersionState::version>>
            >
          >;
        if (s.out())
        {
          VersionsState states;
          if (this->_state)
            states.emplace(this->_state.get());
          s.serialize("state", states);
        }
        else
        {
          VersionsState states;
          s.serialize("state", states);
          auto it = states.rbegin();
          if (it != states.rend())
            this->_state.emplace(*it);
        }
        if (v >= elle::Version(0, 2, 0))
          s.serialize("partial", this->_partial);
      }

      /*----------.
      | Printable |
      `----------*/

      template <
        typename T, typename Version, typename ClientId, typename ServerId>
      void
      Server<T, Version, ClientId, ServerId>::print(std::ostream& output) const
      {
        elle::fprintf(output, "athena::paxos::Server(%f)", this->id());
      }
    }
  }
}
