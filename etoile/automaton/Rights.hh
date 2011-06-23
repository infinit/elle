//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/automaton/Rights.hh
//
// created       julien quintard   [mon jun 20 14:00:01 2011]
// updated       julien quintard   [wed jun 22 16:25:04 2011]
//

#ifndef ETOILE_AUTOMATON_RIGHTS_HH
#define ETOILE_AUTOMATON_RIGHTS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/gear/Object.hh>

namespace etoile
{
  namespace automaton
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides automata for computing the user's rights
    /// over a given object context.
    ///
    class Rights
    {
    public:
      //
      // static methods
      //
      static elle::Status	Determine(gear::Object&);
      static elle::Status	Recompute(gear::Object&);
      static elle::Status	Update(gear::Object&,
				       const nucleus::Permissions&);
    };

  }
}

#endif
