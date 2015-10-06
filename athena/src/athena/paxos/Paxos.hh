#ifndef ATHENA_PAXOS_PAXOS_HH
# define ATHENA_PAXOS_PAXOS_HH

# include <elle/attribute.hh>
# include <elle/Printable.hh>

# include <reactor/Barrier.hh>

namespace athena
{
  namespace paxos
  {
    template <typename T, typename ServerId>
    class Paxos
      : public elle::Printable
    {
    /*------.
    | Types |
    `------*/
    public:
      struct Proposal
      {
        int round;
        ServerId sender;
        bool
        operator <(Proposal const& rhs) const;
      };
      struct Accepted
      {
        Proposal proposal;
        T value;
      };

    /*-----.
    | Peer |
    `-----*/
    public:
      class Peer
      {
      public:
        virtual
        boost::optional<Accepted>
        propose(ServerId const& sender, int round) = 0;
        virtual
        Proposal
        accept(ServerId const& sender, int round, T const& value) = 0;
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
      Paxos(ServerId id, Peers peers);
      ELLE_ATTRIBUTE_R(ServerId, id);
      ELLE_ATTRIBUTE_R(Peers, peers);

    /*----------.
    | Consensus |
    `----------*/
    public:
      T
      choose(T const& value);
      T
      choose();
      boost::optional<Accepted>
      propose(ServerId const& sender, int round);
      Proposal
      accept(ServerId const& sender, int round, T const& value);
      ELLE_ATTRIBUTE(int, round);
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

# include <athena/paxos/Paxos.hxx>

#endif
