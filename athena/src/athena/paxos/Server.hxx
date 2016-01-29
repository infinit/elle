#ifndef ATHENA_PAXOS_SERVER_HXX
# define ATHENA_PAXOS_SERVER_HXX

# include <elle/With.hh>
# include <elle/serialization/Serializer.hh>

# include <reactor/Scope.hh>

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
      return this->version == rhs.version &&
        this->round == rhs.round &&
        this->sender == rhs.sender;
    }

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    bool
    Server<T, Version, ClientId, ServerId>::Proposal::operator <(
      Proposal const& rhs) const
    {
      if (this->version != rhs.version)
        return this->version < rhs.version;
      else if (this->round != rhs.round)
        return this->round < rhs.round;
      else
        return this->sender < rhs.sender;
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
      elle::serialization::SerializerIn& s, elle::Version const& v)
      : proposal()
      , value(Quorum())
    {
      this->serialize(s, v);
    }

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    Server<T, Version, ClientId, ServerId>::Accepted::Accepted(
      Proposal proposal_,
      elle::Option<T, Quorum> value_)
      : proposal(std::move(proposal_))
      , value(std::move(value_))
    {}

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    void
    Server<T, Version, ClientId, ServerId>::Accepted::serialize(
      elle::serialization::Serializer& serializer,
      elle::Version const& version)
    {
      serializer.serialize("proposal", this->proposal);
      if (version >= elle::Version(0, 1, 0))
        serializer.serialize("value", this->value);
      else if (serializer.out())
      {
        auto& s = static_cast<elle::serialization::SerializerOut&>(serializer);
        if (this->value.template is<T>())
          s.serialize("value", this->value.template get<T>());
        else
          ELLE_ABORT("Athena cannot serialize quorum changes pre 0.1.0");
      }
      else
      {
        auto& s = static_cast<elle::serialization::SerializerIn&>(serializer);
        this->value.template emplace<T>(s.deserialize<T>("value"));
      }
    }

    /*------------.
    | WrongQuorum |
    `------------*/

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    Server<T, Version, ClientId, ServerId>::WrongQuorum::WrongQuorum(
      Quorum expected, Quorum effective)
      : elle::Error("wrong quorum")
      , _expected(std::move(expected))
      , _effective(std::move(effective))
    {}

    template <typename T, typename Version, typename CId, typename SId>
    Server<T, Version, CId, SId>::WrongQuorum::WrongQuorum(
      elle::serialization::SerializerIn& input, elle::Version const& v)
      : Super(input)
    {
      this->_serialize(input, v);
    }

    template < typename T, typename Version, typename CId, typename SId>
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
      if (version < elle::Version(0, 1, 0))
      {
        Version dummy = Version();
        s.serialize("version", dummy);
      }
    }

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    const elle::serialization::Hierarchy<elle::Exception>::Register<
      typename Server<T, Version, ClientId, ServerId>::WrongQuorum>
    Server<T, Version, ClientId, ServerId>::
      _register_wrong_quorum_serialization;

    /*------------.
    | PartialState |
    `------------*/

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

    template < typename T, typename Version, typename CId, typename SId>
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
      _register_partial_state_serialization;

    /*--------.
    | Printer |
    `--------*/

    template <typename T>
    struct Printer
    {
      Printer(T const& o_)
        : o(o_)
      {}

      T const& o;
    };

    template <typename T>
    Printer<T>
    printer(T const& o)
    {
      return Printer<T>(o);
    }

    template <typename T>
    std::ostream&
    operator <<(std::ostream& output, Printer<std::shared_ptr<T>> const& p)
    {
      if (p.o)
        output << *p.o;
      else
        output << "nullptr";
      return output;
    }

    template <typename T>
    std::ostream&
    operator <<(std::ostream& output, Printer<std::unique_ptr<T>> const& p)
    {
      if (p.o)
        output << *p.o;
      else
        output << "nullptr";
      return output;
    }

    template <typename T>
    std::ostream&
    operator <<(std::ostream& output, Printer<T> const& p)
    {
      output << p.o;
      return output;
    }

    /*-------------.
    | Construction |
    `-------------*/

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    Server<T, Version, ClientId, ServerId>::Server(ServerId id, Quorum quorum)
      : _id(std::move(id))
      , _quorum_initial(quorum)
      , _value()
      , _state()
    {
      ELLE_ASSERT_CONTAINS(this->_quorum_initial, this->_id);
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
      boost::optional<typename Server<T, T, CId, SId>::Accepted>
      _highest(
        typename Server<T, T, CId, SId>::VersionsState const& versions,
        std::function<bool(
          typename Server<T, T, CId, SId>::VersionState const& v)> p)
      {
        for (auto it = versions.rbegin(); it != versions.rend(); ++it)
        {
          if (p(*it))
            return it->accepted;
        }
        return {};
      }

      static
      void
      check_quorum(Server<T, Version, CId, SId>& self,
                    Quorum q, Version const& v)
      {
        ELLE_LOG_COMPONENT("athena.paxos.Server");
        // FIXME: suboptimal
        auto const* expected = &self._quorum_initial;
        for (auto it = self._state.rbegin(); it != self._state.rend(); ++it)
          if (it->proposal.version < v &&
              it->accepted &&
              it->accepted->value.template is<Quorum>())
          {
            expected = &it->accepted->value.template get<Quorum>();
            break;
          }
        if (q != *expected)
        {
          ELLE_TRACE("quorum is wrong: %s instead of %s", q, *expected);
          throw WrongQuorum(*expected, std::move(q));
        }
      }

      static
      void
      check_confirmed(Server<T, Version, CId, SId>& self, Version const& v)
      {
        for (auto it = self._state.rbegin(); it != self._state.rend(); ++it)
          if (it->proposal.version >= v)
            continue;
          else if (!it->confirmed)
            throw PartialState(it->proposal);
          else
            break;
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
      _Details::check_confirmed(*this, p.version);
      _Details::check_quorum(*this, q, p.version);
      {
        auto highest = this->highest_accepted();
        if (highest && highest->proposal.version > p.version)
        {
          ELLE_DEBUG(
            "%s: refuse proposal for version %s in favor of version %s",
            *this, p.version, highest->proposal.version);
          return highest.get();
        }
      }
      auto it = this->_state.find(p.version);
      if (it == this->_state.end())
      {
        ELLE_DEBUG_SCOPE(
          "%s: accept proposal for new version %s", *this, p.version);
        this->_state.emplace(std::move(p));
        return {};
      }
      else
      {
        if (it->proposal < p)
        {
          ELLE_DEBUG("%s: update minimum proposal for version %s",
                     *this, p.version);
          this->_state.modify(it,
                              [&] (VersionState& s)
                              {
                                s.proposal = std::move(p);
                              });
        }
        return it->accepted;
      }
    }

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    typename Server<T, Version, ClientId, ServerId>::Proposal
    Server<T, Version, ClientId, ServerId>::accept(
      Quorum q, Proposal p, elle::Option<T, Quorum> value)
    {
      ELLE_LOG_COMPONENT("athena.paxos.Server");
      ELLE_TRACE_SCOPE("%s: accept for %s: %s", *this, p, printer(value));
      _Details::check_quorum(*this, q, p.version);
      {
        auto highest = this->highest_accepted();
        if (highest && highest->proposal.version > p.version)
        {
          ELLE_DEBUG(
            "%s: refuse acceptation for version %s in favor of version %s",
            *this, p.version, highest->proposal.version);
          return highest->proposal;
        }
      }
      auto it = this->_state.find(p.version);
      // FIXME: asserts if someone malicious accepts without a proposal
      ELLE_ASSERT(it != this->_state.end());
      auto& version = *it;
      if (!(p < version.proposal))
      {
        if (!version.accepted)
        {
          this->_state.modify(
            it,
            [&] (VersionState& s)
            {
              s.accepted.emplace(std::move(p), std::move(value));
            });
        }
        else
          this->_state.modify(
            it,
            [&] (VersionState& s)
            {
              s.accepted->proposal = std::move(p);
              s.accepted->value = std::move(value);
            });
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
      _Details::check_quorum(*this, q, p.version);
      auto it = this->_state.find(p.version);
      // FIXME: asserts if someone malicious
      ELLE_ASSERT(it != this->_state.end());
      auto& version = *it;
      ELLE_ASSERT(version.accepted);
      if (!version.confirmed)
      {
        this->_state.modify(it, [] (VersionState& s) { s.confirmed = true; });
        if (version.accepted->value.template is<T>())
          this->_value.emplace(version.accepted->value.template get<T>());
      }
    }

    template <typename T, typename Version, typename CId, typename SId>
    boost::optional<typename Server<T, Version, CId, SId>::Accepted>
    Server<T, Version, CId, SId>::get(Quorum q)
    {
      return _Details::_highest(
        this->_state,
        [] (VersionState const& v)
        {
          return v.confirmed && v.accepted &&
            v.accepted->value.template is<T>();
        });
    }

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    boost::optional<typename Server<T, Version, ClientId, ServerId>::Accepted>
    Server<T, Version, ClientId, ServerId>::highest_accepted() const
    {
      return _Details::_highest(
        this->_state, [] (VersionState const& v) { return bool(v.accepted); });
    }

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    boost::optional<typename Server<T, Version, ClientId, ServerId>::Accepted>
    Server<T, Version, ClientId, ServerId>::highest_accepted_value() const
    {
      return _Details::_highest(
        this->_state,
        [] (VersionState const& v)
        {
          return v.accepted && v.accepted->value.template is<T>();
        });
    }

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    Server<T, Version, ClientId, ServerId>::VersionState::VersionState(
      Proposal p, boost::optional<Accepted> a)
      : proposal(std::move(p))
      , accepted(std::move(a))
      , confirmed(false)
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
      if (v >= elle::Version(0, 1, 0))
        s.serialize("confirmed", this->confirmed);
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
      , _quorum_initial()
      , _value()
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
      s.serialize("id", this->_id);
      s.serialize("quorum", this->_quorum_initial);
      if (v >= elle::Version(0, 1, 0))
        s.serialize("value", this->_value);
      s.serialize("state", this->_state);
    }

    /*----------.
    | Printable |
    `----------*/

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    void
    Server<T, Version, ClientId, ServerId>::print(std::ostream& output) const
    {
      elle::fprintf(output, "%s(%s)", elle::type_info(*this), this->id());
    }
  }
}

#endif
