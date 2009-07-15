//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       GPL
//
// file          /home/mycure/infinit/elle/core/Natural.hh
//
// created       julien quintard   [thu nov 29 20:13:16 2007]
// updated       julien quintard   [mon feb 16 14:24:00 2009]
//

#ifndef ELLE_CORE_NATURAL_HH
#define ELLE_CORE_NATURAL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <inttypes.h>

namespace elle
{
  namespace core
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// this is a generic type for positive integers.
    ///
    typedef uint8_t	Natural8;
    typedef uint16_t	Natural16;
    typedef uint32_t	Natural32;
    typedef uint64_t	Natural64;

  }
}

#endif
