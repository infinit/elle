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
      {
      public:
        typedef typename paxos::Server<T, Version, ClientId>::Proposal Proposal;
        typedef typename paxos::Server<T, Version, ClientId>::Accepted Accepted;
        virtual
        ~Peer() = default;
        virtual
        boost::optional<Accepted>
        propose(Quorum const& q, Proposal const& p) = 0;
        virtual
        Proposal
        accept(Quorum const& q, Proposal const& p, T const& value) = 0;
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
      /** Submit \a value as the chosen value.
       *
       *  \param value the submitted value
       *  \return the value that was chosen if not the one we submitted
       */
      boost::optional<T>
      choose(
        Quorum const& q,
        typename elle::_detail::attribute_r_type<T>::type value);
      /** Submit \a value as the chosen value.
       *
       *  \param value the submitted value
       *  \return the value that was chosen if not the one we submitted
       */
      boost::optional<T>
      choose(
        Quorum const& q,
        typename elle::_detail::attribute_r_type<Version>::type version,
        typename elle::_detail::attribute_r_type<T>::type value);
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
