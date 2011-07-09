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
// updated       julien quintard   [fri jul  8 16:33:50 2011]
//

#ifndef ELLE_CORE_POINTER_HH
#define ELLE_CORE_POINTER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

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
    /// \todo make it portable
    ///
    typedef Natural32			Pointer;

  }
}

#endif
