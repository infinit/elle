#ifndef HOLE_IMPLEMENTATIONS_REMOTE_SERVER_HH
# define HOLE_IMPLEMENTATIONS_REMOTE_SERVER_HH

# include <boost/noncopyable.hpp>

# include <unordered_set>

# include <elle/Printable.hh>
# include <elle/types.hh>
# include <elle/radix/Entity.hh>

# include <reactor/fwd.hh>
# include <reactor/network/fwd.hh>

# include <nucleus/fwd.hh>

# include <hole/fwd.hh>
# include <hole/implementations/remote/Customer.hh>

# include <lune/fwd.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {
      /// The server waiting for clients to connect which are then
      /// authenticated.
      ///
      /// Note that multiple clients can connect to the server in
      /// parallel.
      class Server
        : public elle::radix::Entity
        , public boost::noncopyable
        , public elle::Printable
      {
      /*------.
      | Types |
      `------*/
      public:

      /*-------------.
      | Construction |
      `-------------*/
      public:
        Server(Hole& hole, int port);
        ~Server();
      private:
        ELLE_ATTRIBUTE_R(Hole&, hole);
        int _port;

      /*----------.
      | Customers |
      `----------*/
      private:
        /// Set of customers.
        typedef std::unordered_set<Customer*> Customers;
        /// The customers.
        Customers _customers;
        /// Add the given customer to the set.
        void
        _add(Customer*);
        /// Remove a customer from the set.
        void
        _remove(Customer*);
        /// Let customers manage the set.
        friend class Customer;

      /*----.
      | API |
      `----*/
      public:
        /// Store an immutable block.
        void
        put(const nucleus::proton::Address&,
            const nucleus::proton::ImmutableBlock&);
        /// Store a mutable block.
        void
        put(const nucleus::proton::Address&,
            const nucleus::proton::MutableBlock&);
        /// Retrieve an immutable block.
        std::unique_ptr<nucleus::proton::Block>
        get(const nucleus::proton::Address&);
        /// Retrieve a mutable block.
        std::unique_ptr<nucleus::proton::Block>
        get(const nucleus::proton::Address&,
            const nucleus::proton::Revision&);
        /// This method removes a block.
        void
        kill(const nucleus::proton::Address&);


      /*----------.
      | Printable |
      `----------*/
      public:
        virtual
        void
        print(std::ostream& stream) const;

      /*---------.
      | Dumpable |
      `---------*/
      public:
        elle::Status            Dump(const elle::Natural32 = 0) const;

      /*-------.
      | Server |
      `-------*/
      private:
        void _accept();
        std::unique_ptr<reactor::network::TCPServer> _server;
        std::unique_ptr<reactor::Thread> _acceptor;
      };
    }
  }
}

#endif
