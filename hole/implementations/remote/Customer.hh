#ifndef HOLE_IMPLEMENTATIONS_REMOTE_CUSTOMER_HH
# define HOLE_IMPLEMENTATIONS_REMOTE_CUSTOMER_HH

# include <elle/types.hh>
# include <elle/radix/Entity.hh>

# include <elle/concurrency/Signal.hh>
# include <elle/network/fwd.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

      ///
      /// this class represents a client being connected to the server
      /// i.e the machine.
      ///
      class Customer:
        public elle::radix::Entity
      {
      public:
        //
        // constants
        //
        static const elle::Natural32            Timeout;

        //
        // enumerations
        //
        enum State
          {
            StateUnknown,
            StateConnected,
            StateAuthenticated,
            StateDead
          };

        //
        // constructors & destructors
        //
        Customer(elle::network::TCPSocket* socket);
        ~Customer();

        //
        // interfaces
        //

        // dumpable
        elle::Status            Dump(const elle::Natural32 = 0) const;

        //
        // signals
        //
        struct
        {
          elle::concurrency::Signal<
            elle::radix::Parameters<
              Customer*
              >
            >                   dead;
        }                       signal;

        //
        // attributes
        //
        State                   state;

        elle::network::TCPSocket*        socket;
      };

    }
  }
}

#endif
