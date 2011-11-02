//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [mon jun 20 12:45:10 2011]
//

#ifndef ETOILE_AUTOMATON_OBJECT_HH
#define ETOILE_AUTOMATON_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/miscellaneous/Abstract.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// this class needs to be forwarded declared in order to prevent
    /// conflits.
    ///
    class Object;

  }

  namespace automaton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides object-related automata.
    ///
    /// these automata therefore operate on the general abstract such
    /// as the access block referenced by any object, the meta data and so
    /// on.
    ///
    class Object
    {
    public:
      //
      // static methods
      //
      static elle::Status	Load(gear::Object&);

      static elle::Status	Information(gear::Object&,
					    miscellaneous::Abstract&);

      static elle::Status	Discard(gear::Object&);
      static elle::Status	Destroy(gear::Object&);
      static elle::Status	Store(gear::Object&);
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Object.hh>

#endif
