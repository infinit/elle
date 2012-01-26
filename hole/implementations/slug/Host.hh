//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [wed aug 24 10:33:01 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_SLUG_HOST_HH
#define HOLE_IMPLEMENTATIONS_SLUG_HOST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <hole/Label.hh>

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
        Host();
        ~Host();

        //
        // methods
        //
        elle::Status    Create(const elle::Locus&);
        elle::Status    Create(elle::TCPSocket*);

        elle::Status    Connect();
        elle::Status    Disconnect();

        elle::Status    Authenticated();

        //
        // callbacks
        //
        elle::Status    Connected();
        elle::Status    Disconnected();
        elle::Status    Error(const elle::String&);
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

        elle::Timer*            timer;
      };

    }
  }
}

#endif
