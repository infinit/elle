//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/core/Natural.hh
//
// created       julien quintard   [thu nov 29 20:13:16 2007]
// updated       julien quintard   [wed mar 10 20:40:19 2010]
//

#ifndef ELLE_CORE_NATURAL_HH
#define ELLE_CORE_NATURAL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/idiom/Close.hh>
# include <inttypes.h>
#include <elle/idiom/Open.hh>

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
