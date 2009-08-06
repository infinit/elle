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
// updated       julien quintard   [thu aug  6 10:55:14 2009]
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
    /// this type is used internally by some classes to deal with
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
