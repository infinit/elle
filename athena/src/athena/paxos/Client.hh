#ifndef ATHENA_PAXOS_CLIENT_HH
# define ATHENA_PAXOS_CLIENT_HH

# include <elle/attribute.hh>
# include <elle/Printable.hh>

# include <reactor/Barrier.hh>

# include <athena/paxos/Server.hh>

namespace athena
{
  namespace paxos
  {
    template <typename T, typename ClientId>
    class Client
      : public elle::Printable
    {
    /*------.
    | Types |
    `------*/
    public:
      typedef Client<T, ClientId> Self;
      typedef paxos::Server<T, ClientId> Server;
      typedef typename paxos::Server<T, ClientId>::Proposal Proposal;
      typedef typename paxos::Server<T, ClientId>::Accepted Accepted;

    /*-----.
    | Peer |
    `-----*/
    public:
      class Peer
      {
      public:
        virtual
        boost::optional<Accepted>
        propose(Proposal const& p) = 0;
        virtual
        Proposal
        accept(Proposal const& p, T const& value) = 0;
        class Unavailable
          : public elle::Error
        {
        public:
          Unavailable();
        };
      };
      typedef std::vector<std::unique_ptr<Peer>> Peers;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      Client(ClientId id, Peers peers);
      ELLE_ATTRIBUTE_R(ClientId, id);
      ELLE_ATTRIBUTE_R(Peers, peers);

    /*----------.
    | Consensus |
    `----------*/
    public:
      T
      choose(T const& value);
      ELLE_ATTRIBUTE(int, round);

    /*----------.
    | Printable |
    `----------*/
    public:
      void
      print(std::ostream& output) const override;
    };
  }
}

# include <athena/paxos/Client.hxx>

#endif
