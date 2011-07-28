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
// updated       julien quintard   [thu jul 28 14:11:06 2011]
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
      // methods
      //
      elle::Status	Create(Scope*);

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
