//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/core/Pointer.hh
//
// created       julien quintard   [fri jul  8 16:32:05 2011]
// updated       julien quintard   [sun sep  4 15:26:24 2011]
//

#ifndef ELLE_CORE_POINTER_HH
#define ELLE_CORE_POINTER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Void.hh>

namespace elle
{
  namespace core
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// this type defines a memory word which depends upon the
    /// microprocessor architecture.
    ///
    typedef Void*			Pointer;

  }
}

#endif
