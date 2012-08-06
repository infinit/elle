#ifndef HOLE_IMPLEMENTATIONS_REMOTE_SERVER_HH
# define HOLE_IMPLEMENTATIONS_REMOTE_SERVER_HH

# include <boost/noncopyable.hpp>

# include <unordered_set>

# include <elle/types.hh>
# include <elle/radix/Entity.hh>

# include <reactor/fwd.hh>
# include <reactor/network/fwd.hh>

# include <nucleus/fwd.hh>

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
      {
      /*------.
      | Types |
      `------*/
      public:

      /*-------------.
      | Construction |
      `-------------*/
      public:
        Server(int port);
        ~Server();

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
        void
        get(const nucleus::proton::Address&,
            nucleus::proton::ImmutableBlock&);
        /// Retrieve a mutable block.
        void
        get(const nucleus::proton::Address&,
            const nucleus::proton::Version&,
            nucleus::proton::MutableBlock&);
        /// This method removes a block.
        void
        kill(const nucleus::proton::Address&);

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
