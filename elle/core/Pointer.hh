//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri jul  8 16:32:05 2011]
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
