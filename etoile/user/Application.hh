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
// updated       julien quintard   [wed apr 21 23:26:53 2010]
//

#ifndef ETOILE_USER_APPLICATION_HH
#define ETOILE_USER_APPLICATION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/context/Identifier.hh>

namespace etoile
{
  namespace context
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX
    ///
    class Context;

  }

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
      // types
      //
      typedef std::map<const context::Identifier,
		       context::Context*>		Container;
      typedef Container::iterator			Iterator;
      typedef Container::const_iterator			Scoutor;

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

      template <typename T>
      Status		Add(const context::Identifier&,
			    T*);
      template <typename T>
      Status		Retrieve(const context::Identifier&,
				 T*&);
      Status		Remove(const context::Identifier&);

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
      Channel*		channel;

      Container		contexts;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Context.hh>

//
// ---------- templates -------------------------------------------------------
//

#include <etoile/user/Application.hxx>

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/user/Client.hh>
#include <etoile/user/Map.hh>

#endif
