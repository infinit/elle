#ifndef HOLE_IMPLEMENTATIONS_REMOTE_CUSTOMER_HH
# define HOLE_IMPLEMENTATIONS_REMOTE_CUSTOMER_HH

# include <elle/types.hh>

# include <elle/radix/Entity.hh>
# include <elle/concurrency/Signal.hh>
# include <elle/concurrency/fwd.hh>

# include <elle/network/fwd.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

//
// ---------- classes ---------------------------------------------------------
//

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
        Customer();
        ~Customer();

        //
        // methods
        //
        elle::Status            Create(elle::network::TCPSocket*);

        //
        // callbacks
        //
        elle::Status            Disconnected();
        // XXX: const string&
        elle::Status            Error(elle::String);

        elle::Status            Abort();

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
        elle::concurrency::Timer*            timer;
      };

    }
  }
}

#endif
