//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [mon jun 20 14:00:01 2011]
//

#ifndef ETOILE_AUTOMATON_RIGHTS_HH
#define ETOILE_AUTOMATON_RIGHTS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

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
      static elle::Status       Determine(gear::Object&);
      static elle::Status       Recompute(gear::Object&);
      static elle::Status       Update(gear::Object&,
                                       const nucleus::Permissions&);

      static elle::Status       Operate(gear::Object&,
                                        const gear::Operation&);
    };

  }
}

#endif
