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
// updated       julien quintard   [fri aug  5 12:43:04 2011]
//

#ifndef ETOILE_GEAR_ACTOR_HH
#define ETOILE_GEAR_ACTOR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Operation.hh>

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
      // enumerations
      //
      enum State
	{
	  StateClean,
	  StateUpdated
	};

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

      elle::Status	Operate(const Operation);

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
      State		state;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Scope.hh>

#endif
