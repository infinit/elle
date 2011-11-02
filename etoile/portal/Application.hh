//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien.quintard   [tue nov  1 08:38:35 2011]
//

#ifndef ETOILE_PORTAL_APPLICATION_HH
#define ETOILE_PORTAL_APPLICATION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace etoile
{
  namespace portal
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents an external client triggering operations
    /// on Etoile.
    ///
    /// note that the client must authenticate to Etoile in order to
    /// use the message interface.
    ///
    class Application:
      public elle::Entity
    {
    public:
      //
      // constants
      //
      static const elle::Natural32		Timeout;

      //
      // enumerations
      //
      enum State
	{
	  StateConnected,
	  StateAuthenticated,
	  StateDisconnected
	};

      //
      // constructors & destructors
      //
      Application();
      ~Application();

      //
      // methods
      //
      elle::Status	Create(elle::Door*);

      //
      // callbacks
      //
      elle::Status	Disconnected();
      elle::Status	Error(const elle::String&);
      elle::Status	Abort();

      //
      // attributes
      //
      State		state;
      elle::Timer*	timer;

      elle::Door*	door;
    };

  }
}

#endif
