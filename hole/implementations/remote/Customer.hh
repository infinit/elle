#ifndef HOLE_IMPLEMENTATIONS_REMOTE_CUSTOMER_HH
# define HOLE_IMPLEMENTATIONS_REMOTE_CUSTOMER_HH

# include <elle/Elle.hh>

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
        public elle::Entity
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
        Customer(elle::TCPSocket* socket);
        ~Customer();

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
          elle::Signal<
            elle::Parameters<
              Customer*
              >
            >                   dead;
        }                       signal;

        //
        // attributes
        //
        State                   state;

        elle::TCPSocket*        socket;
      };

    }
  }
}

#endif
