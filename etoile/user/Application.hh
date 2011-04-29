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
// updated       julien quintard   [tue mar  1 14:25:21 2011]
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
      public elle::Entity
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
      elle::Status	Create(elle::Channel*);
      elle::Status	Destroy();

      template <typename T>
      elle::Status	Add(const context::Identifier&,
			    T*);
      template <typename T>
      elle::Status	Retrieve(const context::Identifier&,
				 T*&);
      elle::Status	Remove(const context::Identifier&);

      //
      // callbacks
      //
      elle::Status	Timeout();
      elle::Status	Error(const elle::String&);

      //
      // interfaces
      //

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      // XXX hole::Universe	universe;

      elle::Channel*	channel;

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

#endif
