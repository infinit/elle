//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/user/Application.hh
//
// created       julien quintard   [thu mar 11 17:09:50 2010]
// updated       julien quintard   [thu mar 25 18:00:09 2010]
//

#ifndef ETOILE_USER_APPLICATION_HH
#define ETOILE_USER_APPLICATION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace etoile
{
  namespace user
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents an application connected to Etoile
    /// and requesting operations.
    ///
    class Application:
      public Dumpable
    {
    public:
      //
      // constants
      //
      static const Natural32		Expiration;

      //
      // enumerations
      //
      enum State
	{
	  StateUnconnected,
	  StateConnected
	};

      //
      // constructors & destructors
      //
      Application();
      ~Application();

      //
      // methods
      //
      Status		Create(Channel*);
      Status		Destroy();

      //
      // callbacks
      //
      Status		Timeout();
      Status		Error(const String&);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      State		state;
      Timer		timer;
      Channel*		channel;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/Client.hh>
#include <etoile/user/Map.hh>

#endif
