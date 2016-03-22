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
    template <typename T, typename Version, typename ClientId>
    class Client
      : public elle::Printable
    {
    /*------.
    | Types |
    `------*/
    public:
      typedef Client<T, Version, ClientId> Self;
      typedef paxos::Server<T, Version, ClientId> Server;
      typedef typename paxos::Server<T, Version, ClientId>::Accepted Accepted;
      typedef typename paxos::Server<T, Version, ClientId>::Proposal Proposal;
      typedef typename paxos::Server<T, Version, ClientId>::Quorum Quorum;

    /*-----.
    | Peer |
    `-----*/
    public:
      class Peer
        : public elle::Printable
      {
      public:
        typedef typename paxos::Server<T, Version, ClientId>::Proposal Proposal;
        typedef typename paxos::Server<T, Version, ClientId>::Accepted Accepted;
        Peer(ClientId id);
        virtual
        ~Peer() = default;
        virtual
        boost::optional<Accepted>
        propose(Quorum const& q, Proposal const& p) = 0;
        virtual
        Proposal
        accept(Quorum const& q, Proposal const& p,
               elle::Option<T, Quorum> const& value) = 0;
        virtual
        void
        confirm(Quorum const& q, Proposal const& p) = 0;
        virtual
        boost::optional<Accepted>
        get(Quorum const& q) = 0;
        class Unavailable
          : public elle::Error
        {
        public:
          Unavailable();
        };
        ELLE_ATTRIBUTE_R(ClientId, id);
      /*----------.
      | Printable |
      `----------*/
      public:
        void
        print(std::ostream& output) const override;
      };
      typedef std::vector<std::unique_ptr<Peer>> Peers;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      Client(ClientId id, Peers peers);
      void
      peers(Peers peers);
      ELLE_ATTRIBUTE_R(ClientId, id);
      // FIXME: the W is there only for unit tests
      ELLE_ATTRIBUTE_RX(Peers, peers);
      ELLE_ATTRIBUTE_RW(bool, conflict_backoff);

    /*----------.
    | Consensus |
    `----------*/
    public:
      /** Submit \a value as the chosen value.
       *
       *  \param value the submitted value
       *  \return the value that was chosen if not the one we submitted
       */
      boost::optional<Accepted>
      choose(elle::Option<T, Quorum> const& value);
      /** Submit \a value as the chosen value.
       *
       *  \param value the submitted value
       *  \return the value that was chosen if not the one we submitted
       */
      boost::optional<Accepted>
      choose(typename elle::_detail::attribute_r_type<Version>::type version,
             elle::Option<T, Quorum> const& value);
      /** Get the latest chosen value.
       */
      boost::optional<T>
      get();
      std::pair<boost::optional<T>, Quorum>
      get_quorum();
      ELLE_ATTRIBUTE(int, round);
    private:
      /** Check a majority of members where reached.
       *
       *  \param q The quorum we consult.
       *  \param reached The number of member successfully consulted.
       *  \throws TooFewPeers if a strict majority was not reached.
       */
      void
      _check_headcount(Quorum const& q,
                       int reached,
                       bool reading = false) const;

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
