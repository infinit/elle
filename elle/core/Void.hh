//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/core/Void.hh
//
// created       julien quintard   [thu jul 30 13:26:57 2009]
// updated       julien quintard   [thu jul 30 13:28:28 2009]
//

#ifndef ELLE_CORE_VOID_HH
#define ELLE_CORE_VOID_HH


//
// ---------- includes --------------------------------------------------------
//

#include <ostream>

namespace elle
{
  namespace core
  {

//
// ---------- structures ------------------------------------------------------
//

    ///
    /// this type is used internally by some classes for dealing with
    /// variadic templates.
    ///
    struct Void
    {
    };

//
// ---------- declarations ----------------------------------------------------
//

    extern Void			vacuum;

  }
}

#endif
