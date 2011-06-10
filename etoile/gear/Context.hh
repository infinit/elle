//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Context.hh
//
// created       julien quintard   [fri aug 14 22:36:10 2009]
// updated       julien quintard   [fri jun  3 11:35:55 2011]
//

#ifndef ETOILE_GEAR_CONTEXT_HH
#define ETOILE_GEAR_CONTEXT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// a context keeps a component's original state.
    ///
    /// note that this class is merely an abstraction as, depending on
    /// the component, the related blocks differ.
    ///
    class Context:
      public elle::Object
    {
    };

  }
}

#endif
