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
// updated       julien quintard   [sun feb 28 10:47:04 2010]
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
    /// this type represents the absence of type.
    ///
    typedef void		Void;

//
// ---------- declarations ----------------------------------------------------
//

    extern Void*		vacuum;

  }
}

#endif
