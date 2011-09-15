//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu nov 29 20:13:16 2007]
//

#ifndef ELLE_CORE_NATURAL_HH
#define ELLE_CORE_NATURAL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/idiom/Close.hh>
# include <ostream>
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
    typedef uint8_t		Natural8;
    typedef uint16_t		Natural16;
    typedef uint32_t		Natural32;
    typedef uint64_t		Natural64;

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  ostream&		operator<<(ostream&,
				   const elle::core::Natural8&);

}

#endif
