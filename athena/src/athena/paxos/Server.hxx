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

    template <typename T, typename Version, typename ClientId>
    Server<T, Version, ClientId>::Proposal::Proposal(
      Version version_, int round_, ClientId sender_)
      : version(version_)
      , round(round_)
      , sender(sender_)
    {}

    template <typename T, typename Version, typename ClientId>
    Server<T, Version, ClientId>::Proposal::Proposal()
      : version()
      , round()
      , sender()
    {}

    template <typename T, typename Version, typename ClientId>
    bool
    Server<T, Version, ClientId>::Proposal::operator <(
      Proposal const& rhs) const
    {
      if (this->round < rhs.round)
        return true;
      return this->sender < rhs.sender;
    }

    template <typename T, typename Version, typename ClientId>
    void
    Server<T, Version, ClientId>::Proposal::serialize(
      elle::serialization::Serializer& s)
    {
      s.serialize("version", this->version);
      s.serialize("round", this->round);
      s.serialize("sender", this->sender);
    }

    template <typename T, typename Version, typename ClientId>
    Server<T, Version, ClientId>::Accepted::Accepted()
      : proposal()
      , value()
    {}

    template <typename T, typename Version, typename ClientId>
    Server<T, Version, ClientId>::Accepted::Accepted(
      Proposal proposal_, T value_)
      : proposal(std::move(proposal_))
      , value(std::move(value_))
    {}

    template <typename T, typename Version, typename ClientId>
    void
    Server<T, Version, ClientId>::Accepted::serialize(
      elle::serialization::Serializer& s)
    {
      s.serialize("proposal", this->proposal);
      s.serialize("value", this->value);
    }

    template <typename T, typename Version, typename ClientId>
    Server<T, Version, ClientId>::VersionState::VersionState(Proposal p)
      : proposal(std::move(p))
      , accepted()
    {}

    template <typename T, typename Version, typename ClientId>
    Version
    Server<T, Version, ClientId>::VersionState::version() const
    {
      return this->proposal.version;
    }

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

    template <typename T, typename Version, typename ClientId>
    Server<T, Version, ClientId>::Server()
      : _state()
    {}

    template <typename T, typename Version, typename ClientId>
    boost::optional<typename Server<T, Version, ClientId>::Accepted>
    Server<T, Version, ClientId>::propose(Proposal p)
    {
      ELLE_LOG_COMPONENT("athena.paxos.Server");
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

    template <typename T, typename Version, typename ClientId>
    typename Server<T, Version, ClientId>::Proposal
    Server<T, Version, ClientId>::accept(Proposal p, T value)
    {
      ELLE_LOG_COMPONENT("athena.paxos.Server");
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
          this->_state.modify(
            it,
            [&] (VersionState& s)
            {
              s.accepted.emplace(std::move(p), std::move(value));
            });
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

    template <typename T, typename Version, typename ClientId>
    boost::optional<typename Server<T, Version, ClientId>::Accepted>
    Server<T, Version, ClientId>::highest_accepted() const
    {
      for (auto it = this->_state.rbegin(); it != this->_state.rend(); ++it)
      {
        if (it->accepted)
          return it->accepted;
      }
      return {};
    }

    /*----------.
    | Printable |
    `----------*/

    template <typename T, typename Version, typename ClientId>
    void
    Server<T, Version, ClientId>::print(std::ostream& output) const
    {
      elle::fprintf(output, "paxos::Server(%x)", this);
    }
  }
}

#endif
