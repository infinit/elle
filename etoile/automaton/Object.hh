//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/automaton/Object.hh
//
// created       julien quintard   [mon jun 20 12:45:10 2011]
// updated       julien quintard   [wed jun 22 14:07:15 2011]
//

#ifndef ETOILE_AUTOMATON_OBJECT_HH
#define ETOILE_AUTOMATON_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/gear/Object.hh>

#include <etoile/miscellaneous/Information.hh>

namespace etoile
{
  namespace automaton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides object-related automata.
    ///
    /// these automata therefore operate on the general information such
    /// as the access block referenced by any object, the meta data and so
    /// on.
    ///
    class Object
    {
    public:
      //
      // static methods
      //
      static elle::Status	Load(gear::Object&,
				     const nucleus::Location&);

      static elle::Status	Information(gear::Object&,
					    miscellaneous::Information&);

      static elle::Status	Destroy(gear::Object&);
      static elle::Status	Store(gear::Object&);
    };

  }
}

#endif
