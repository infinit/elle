//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/core/Integer.hh
//
// created       julien quintard   [thu nov 29 20:13:16 2007]
// updated       julien quintard   [mon feb 16 14:24:06 2009]
//

#ifndef ELLE_CORE_INTEGER_HH
#define ELLE_CORE_INTEGER_HH

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
    /// this is a generic type for negative and positive integers.
    ///
    typedef int8_t	Integer8;
    typedef int16_t	Integer16;
    typedef int32_t	Integer32;
    typedef int64_t	Integer64;

  }
}

#endif
