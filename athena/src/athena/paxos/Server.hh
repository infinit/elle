#ifndef ATHENA_PAXOS_SERVER_HH
# define ATHENA_PAXOS_SERVER_HH

# include <unordered_set>

# include <boost/multi_index_container.hpp>
# include <boost/multi_index/ordered_index.hpp>
# include <boost/multi_index/member.hpp>

# include <elle/attribute.hh>
# include <elle/Error.hh>
# include <elle/Option.hh>
# include <elle/Printable.hh>
# include <elle/serialization/Serializer.hh>

# include <reactor/Barrier.hh>

namespace athena
{
  namespace paxos
  {
    template <typename T,
              typename Version,
              typename ClientId,
              typename ServerId = ClientId>
    class Server
      : public elle::Printable
    {
    /*------.
    | Types |
    `------*/
    public:
      typedef Server<T, Version, ClientId, ServerId> Self;
      typedef std::unordered_set<ServerId> Quorum;

    /*---------.
    | Proposal |
    `---------*/
    public:
      struct Proposal
      {
        Proposal();
        Proposal(Version version, int round, ClientId sender);
        Version version;
        int round;
        ClientId sender;
        bool
        operator ==(Proposal const& rhs) const;
        bool
        operator <(Proposal const& rhs) const;
        void
        serialize(elle::serialization::Serializer& s);
        typedef elle::serialization_tag serialization_tag;
        friend
        std::ostream&
        operator <<(std::ostream& output,
                    typename Server<T, Version, ClientId>::Proposal const& p)
        {
          output << p.version << ":" << p.round << ":" << p.sender;
          return output;
        }
      };

    /*---------.
    | Accepted |
    `---------*/
    public:
      struct Accepted
      {
        Accepted(elle::serialization::SerializerIn& s, elle::Version const& v);
        Accepted(Proposal proposal, elle::Option<T, Quorum> value);
        Proposal proposal;
        elle::Option<T, Quorum> value;
        void
        serialize(elle::serialization::Serializer& s, elle::Version const& v);
        typedef elle::serialization_tag serialization_tag;
      };

    /*------------.
    | WrongQuorum |
    `------------*/
    public:
      class WrongQuorum
        : public elle::Error
      {
      public:
        typedef elle::Error Super;
        WrongQuorum(Quorum expected, Quorum effective, Proposal proposal);
        WrongQuorum(elle::serialization::SerializerIn& input,
                    elle::Version const& version);
        virtual
        void
        serialize(elle::serialization::Serializer& s,
                  elle::Version const& version) override;
        ELLE_ATTRIBUTE_R(Quorum, expected);
        ELLE_ATTRIBUTE_R(Quorum, effective);
        ELLE_ATTRIBUTE_R(Proposal, proposal);
      private:
        void
        _serialize(elle::serialization::Serializer& s,
                   elle::Version const& version);
      };
    private:
      static const elle::serialization::Hierarchy<elle::Exception>::
      Register<WrongQuorum> _register_serialization;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      Server(ServerId id, Quorum quorum);
      ELLE_ATTRIBUTE_R(ServerId, id);
      ELLE_ATTRIBUTE_R(Quorum, quorum);
      ELLE_ATTRIBUTE_R(boost::optional<T>, value);

    /*----------.
    | Consensus |
    `----------*/
    public:
      boost::optional<Accepted>
      propose(Quorum q, Proposal p);
      Proposal
      accept(Quorum q, Proposal p, elle::Option<T, Quorum> value);
      boost::optional<Accepted>
      highest_accepted() const;
      boost::optional<Accepted>
      highest_accepted_value() const;
    private:
      struct _Details;
      friend struct _Details;
      struct VersionState
      {
        VersionState(Proposal p, boost::optional<Accepted> a = {});
        Proposal proposal;
        boost::optional<Accepted> accepted;
        Version
        version() const;
        VersionState(elle::serialization::SerializerIn& s);
        void
        serialize(elle::serialization::Serializer& s);
        typedef elle::serialization_tag serialization_tag;
      };
      typedef boost::multi_index::multi_index_container<
        VersionState,
        boost::multi_index::indexed_by<
          boost::multi_index::ordered_unique<
            boost::multi_index::const_mem_fun<
              VersionState, Version, &VersionState::version>>
          >
        > VersionsState;
      ELLE_ATTRIBUTE(VersionsState, state);
      void
      _check_quorum(Quorum q, Version const& version) const;

    /*--------------.
    | Serialization |
    `--------------*/
    public:
      Server(elle::serialization::SerializerIn& s);
      void
      serialize(elle::serialization::Serializer& s);
      typedef elle::serialization_tag serialization_tag;

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
