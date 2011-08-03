//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Actor.hh
//
// created       julien quintard   [thu jul 28 12:45:43 2011]
// updated       julien quintard   [sat jul 30 13:30:25 2011]
//

#ifndef ETOILE_GEAR_ACTOR_HH
#define ETOILE_GEAR_ACTOR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/gear/Identifier.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX
    ///
    class Scope;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Actor:
      public elle::Object
    {
    public:
      //
      // types
      //
      typedef std::map<const Identifier,
		       Actor*>				Container;
      typedef Container::iterator			Iterator;
      typedef Container::const_iterator			Scoutor;

      //
      // methods
      //
      static elle::Status	Add(const Identifier&,
				    Actor*);
      static elle::Status	Select(const Identifier&,
				       Actor*&);
      static elle::Status	Remove(const Identifier&);

      //
      // static attributes
      //
      static Container		Actors;

      //
      // constructors & destructors
      //
      Actor(Scope*);

      //
      // methods
      //
      elle::Status	Attach();
      elle::Status	Detach();

      //
      // interfaces
      //

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Identifier	identifier;
      Scope*		scope;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Scope.hh>

#endif
