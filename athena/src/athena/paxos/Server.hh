#ifndef ATHENA_PAXOS_SERVER_HH
# define ATHENA_PAXOS_SERVER_HH

# include <boost/multi_index_container.hpp>
# include <boost/multi_index/ordered_index.hpp>
# include <boost/multi_index/member.hpp>

# include <elle/attribute.hh>
# include <elle/Printable.hh>

# include <reactor/Barrier.hh>

namespace athena
{
  namespace paxos
  {
    template <typename T, typename Version, typename ClientId>
    class Server
      : public elle::Printable
    {
    /*------.
    | Types |
    `------*/
    public:
      struct Proposal
      {
        Proposal();
        Proposal(Version version, int round, ClientId sender);
        Version version;
        int round;
        ClientId sender;
        bool
        operator <(Proposal const& rhs) const;
        void
        serialize(elle::serialization::Serializer& s);
        friend
        std::ostream&
        operator <<(std::ostream& output,
                    typename Server<T, Version, ClientId>::Proposal const& p)
        {
          output << p.version << ":" << p.round << ":" << p.sender;
          return output;
        }
      };

      struct Accepted
      {
        Accepted();
        Accepted(Proposal proposal, T value);
        Proposal proposal;
        T value;
        void
        serialize(elle::serialization::Serializer& s);
      };

      struct VersionState
      {
        VersionState(Proposal p);
        Proposal proposal;
        boost::optional<Accepted> accepted;
        Version
        version() const;
      };

    /*-------------.
    | Construction |
    `-------------*/
    public:
      Server();

    /*----------.
    | Consensus |
    `----------*/
    public:
      boost::optional<Accepted>
      propose(Proposal p);
      Proposal
      accept(Proposal p, T value);
      typedef boost::multi_index::multi_index_container<
        VersionState,
        boost::multi_index::indexed_by<
          boost::multi_index::ordered_unique<
            boost::multi_index::const_mem_fun<
              VersionState, Version, &VersionState::version>>
          >
        > VersionsState;
      boost::optional<Accepted>
      highest_accepted() const;
      ELLE_ATTRIBUTE(VersionsState, state);

    /*----------.
    | Printable |
    `----------*/
    public:
      void
      print(std::ostream& output) const override;
    };
  }
}

# include <athena/paxos/Server.hxx>

#endif
