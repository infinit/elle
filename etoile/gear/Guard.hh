//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [Mon 06 Feb 2012 03:55:00 PM CET]
//

#ifndef ETOILE_GEAR_GUARD_HH
#define ETOILE_GEAR_GUARD_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <etoile/gear/Scope.hh>
#include <etoile/gear/Actor.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class enables one to easily manipulate scopes and actor so
    /// as to make sure they get deleted if an error occurs.
    ///
    class Guard
    {
      //
      // constructors & destructors
      //
    public:
      Guard(Scope*,
            Actor* = nullptr);
      Guard(Actor*);
      ~Guard();

      //
      // methods
      //
    public:
      elle::Status      Release();

      //
      // getters & setters
      //
      Actor*            actor();
      elle::Void        actor(Actor*);

      //
      // attributes
      //
    private:
      Scope*            _scope;
      Actor*            _actor;
      elle::Boolean     _track;
    };

  }
}

#endif
