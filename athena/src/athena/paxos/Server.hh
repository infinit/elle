#ifndef ATHENA_PAXOS_SERVER_HH
# define ATHENA_PAXOS_SERVER_HH

# include <elle/attribute.hh>
# include <elle/Printable.hh>

# include <reactor/Barrier.hh>

namespace athena
{
  namespace paxos
  {
    template <typename T, typename ClientId>
    class Server
      : public elle::Printable
    {
    /*------.
    | Types |
    `------*/
    public:
      struct Proposal
      {
        int round;
        ClientId sender;
        bool
        operator <(Proposal const& rhs) const;
        void
        serialize(elle::serialization::Serializer& s);
      };
      struct Accepted
      {
        Proposal proposal;
        T value;
        void
        serialize(elle::serialization::Serializer& s);
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
      propose(Proposal const& p);
      Proposal
      accept(Proposal const& p, T const& value);
      ELLE_ATTRIBUTE(boost::optional<Accepted>, accepted);
      ELLE_ATTRIBUTE(boost::optional<Proposal>, minimum);
      ELLE_ATTRIBUTE(reactor::Barrier, has_accepted);
      ELLE_ATTRIBUTE(bool, chosen);

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
