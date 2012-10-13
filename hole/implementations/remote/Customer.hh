#ifndef HOLE_IMPLEMENTATIONS_REMOTE_CUSTOMER_HH
# define HOLE_IMPLEMENTATIONS_REMOTE_CUSTOMER_HH

# include <boost/noncopyable.hpp>

# include <elle/network/fwd.hh>
# include <elle/radix/Entity.hh>
# include <elle/types.hh>

# include <reactor/network/socket.hh>

# include <protocol/ChanneledStream.hh>
# include <protocol/Serializer.hh>

# include <hole/implementations/remote/fwd.hh>
# include <hole/implementations/remote/Manifest.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {
      /// A client connected to the server i.e the machine.
      class Customer
        : public elle::radix::Entity
        , public boost::noncopyable
      {
      /*----------.
      | Constants |
      `----------*/
      public:
        /// The time after which a customer should have authenticated.
        static const elle::Natural32            Timeout;

      /*-------------.
      | Construction |
      `-------------*/
      public:
        ~Customer();
      private:
        Customer(Server& server,
                 std::unique_ptr<reactor::network::Socket> socket);

      /*---------.
      | Dumpable |
      `---------*/
      public:
        elle::Status            Dump(const elle::Natural32 = 0) const;

      /*----.
      | API |
      `----*/
      public:
        bool
        challenge(lune::Passport const& passport);
        void
        push(const nucleus::proton::Address& address,
             nucleus::Derivable const& derivable);
        nucleus::Derivable
        pull(const nucleus::proton::Address& address,
             const nucleus::proton::Revision& revision);
        void
        wipe(const nucleus::proton::Address&);

      /*----------------.
      | Pretty printing |
      `----------------*/
      public:
        void print(std::ostream& s) const;

      /*------.
      | State |
      `------*/
      private:
        enum class State
          {
            authenticated,
            connected,
          };
        State _state;
        void _state_check_authenticated() const;


      /*--------.
      | Details |
      `--------*/
      private:
        Server& _server;
        friend class Server;
        void _run();
        std::unique_ptr<reactor::network::Socket> _socket;
        infinit::protocol::Serializer _serializer;
        infinit::protocol::ChanneledStream _channels;
        RPC _rpcs;
        reactor::Thread* _runner;
      };

      std::ostream& operator << (std::ostream& s, Customer const& c);
    }
  }
}

#endif
