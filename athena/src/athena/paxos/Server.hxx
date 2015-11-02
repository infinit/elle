#ifndef ATHENA_PAXOS_SERVER_HXX
# define ATHENA_PAXOS_SERVER_HXX

# include <elle/With.hh>
# include <elle/serialization/Serializer.hh>

# include <reactor/Scope.hh>

namespace athena
{
  namespace paxos
  {
    /*------.
    | Types |
    `------*/

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
    Server<T, Version, ClientId, ServerId>::Proposal::operator <(
      Proposal const& rhs) const
    {
      if (this->round < rhs.round)
        return true;
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

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    Server<T, Version, ClientId, ServerId>::Accepted::Accepted()
      : proposal()
      , value()
    {}

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    Server<T, Version, ClientId, ServerId>::Accepted::Accepted(
      Proposal proposal_, T value_)
      : proposal(std::move(proposal_))
      , value(std::move(value_))
    {}

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    void
    Server<T, Version, ClientId, ServerId>::Accepted::serialize(
      elle::serialization::Serializer& s)
    {
      s.serialize("proposal", this->proposal);
      s.serialize("value", this->value);
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
      elle::serialization::SerializerIn& s)
    {
      this->serialize(s);
    }

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    void
    Server<T, Version, ClientId, ServerId>::VersionState::serialize(
      elle::serialization::Serializer& s)
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

    /*------------.
    | WrongQuorum |
    `------------*/

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    Server<T, Version, ClientId, ServerId>::WrongQuorum::WrongQuorum(
      Quorum expected, Quorum effective, Version version)
      : elle::Error(elle::sprintf("wrong quorum, current version is %s", version))
      , _expected(std::move(expected))
      , _effective(std::move(effective))
      , _version(std::move(version))
    {}

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    Server<T, Version, ClientId, ServerId>::WrongQuorum::WrongQuorum(
      elle::serialization::SerializerIn& input)
      : Super(input)
    {
      this->serialize(input);
    }

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    void
    Server<T, Version, ClientId, ServerId>::WrongQuorum::serialize(
      elle::serialization::Serializer& s)
    {
      Super::serialize(s);
      s.serialize("expected", this->_expected);
      s.serialize("effective", this->_effective);
      s.serialize("version", this->_version);
    }

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    const elle::serialization::Hierarchy<elle::Exception>::Register<
      typename Server<T, Version, ClientId, ServerId>::WrongQuorum>
    Server<T, Version, ClientId, ServerId>::_register_serialization;

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
      output << *p.o;
      return output;
    }

    template <typename T>
    std::ostream&
    operator <<(std::ostream& output, Printer<std::unique_ptr<T>> const& p)
    {
      output << *p.o;
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
    Server<T, Version, ClientId, ServerId>::Server(
      ServerId id, Quorum quorum)
      : _id(std::move(id))
      , _quorum(std::move(quorum))
      , _state()
    {
      this->_quorum.insert(this->_id);
      this->_register_serialization.poke();
    }

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    Server<T, Version, ClientId, ServerId>::Server(VersionsState state)
      : _state(std::move(state))
    {}

    /*----------.
    | Consensus |
    `----------*/

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    boost::optional<typename Server<T, Version, ClientId, ServerId>::Accepted>
    Server<T, Version, ClientId, ServerId>::propose(Quorum q, Proposal p)
    {
      ELLE_LOG_COMPONENT("athena.paxos.Server");
      this->_check_quorum(q);
      ELLE_TRACE_SCOPE("%s: get proposal: %s ", *this, p);
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
        ELLE_DEBUG("%s: accept proposal for version %s", *this, p.version);
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
      Quorum q, Proposal p, T value)
    {
      ELLE_LOG_COMPONENT("athena.paxos.Server");
      this->_check_quorum(q);
      ELLE_TRACE_SCOPE("%s: accept for %s: %s", *this, p, printer(value));
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
          // Drop older versions
          for (auto obsolete = this->_state.begin(); obsolete != it;)
            obsolete = this->_state.erase(obsolete);
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
    boost::optional<typename Server<T, Version, ClientId, ServerId>::Accepted>
    Server<T, Version, ClientId, ServerId>::highest_accepted() const
    {
      for (auto it = this->_state.rbegin(); it != this->_state.rend(); ++it)
      {
        if (it->accepted)
          return it->accepted;
      }
      return {};
    }

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    void
    Server<T, Version, ClientId, ServerId>::_check_quorum(Quorum q) const
    {
      ELLE_LOG_COMPONENT("athena.paxos.Server");
      if (q != this->_quorum)
      {
        ELLE_TRACE("quorum is wrong: %s instead of %s", q, this->_quorum);
        throw WrongQuorum(this->_quorum, q, 0);
      }
    }

    /*--------------.
    | Serialization |
    `--------------*/

    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    Server<T, Version, ClientId, ServerId>::Server(
      elle::serialization::SerializerIn& s)
      : _state()
    {
      this->serialize(s);
    }

    // FIXME: use splitted serialization
    template <
      typename T, typename Version, typename ClientId, typename ServerId>
    void
    Server<T, Version, ClientId, ServerId>::serialize(
      elle::serialization::Serializer& s)
    {
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
      elle::fprintf(output, "paxos::Server(%x)", this);
    }
  }
}

#endif
