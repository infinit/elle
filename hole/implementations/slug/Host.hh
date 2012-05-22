#ifndef HOLE_IMPLEMENTATIONS_SLUG_HOST_HH
# define HOLE_IMPLEMENTATIONS_SLUG_HOST_HH

# include <elle/Elle.hh>

# include <hole/Label.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      class Host:
        public elle::Entity
      {
      public:
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
        // constants
        //
        static const elle::Natural32            Timeout;

        //
        // constructors & destructors
        //
        Host(const elle::Locus& locus);
        Host(elle::TCPSocket* connection);
        ~Host();

        //
        // methods
        //
        elle::Status    Disconnect();

        elle::Status    Authenticated();

        //
        // callbacks
        //
        elle::Status    Connected();
        elle::Status    Disconnected();
        // XXX: const string&
        elle::Status    Error(elle::String);
        elle::Status    Abort();

        //
        // interfaces
        //

        // dumpable
        elle::Status    Dump(const elle::Natural32 = 0) const;

        //
        // signals
        //
        struct
        {
          elle::Signal<
            elle::Parameters<
              Host*
              >
            >                   dead;
        }                       signal;

        //
        // attributes
        //
        State                   state;

        elle::Locus             locus;

        elle::TCPSocket*        socket;
      };

    }
  }
}

#endif
