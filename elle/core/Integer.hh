//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu nov 29 20:13:16 2007]
//

#ifndef ELLE_CORE_INTEGER_HH
#define ELLE_CORE_INTEGER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/idiom/Close.hh>
# include <ostream>
# include <cstdint>
#include <elle/idiom/Open.hh>

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
    typedef int8_t              Integer8;
    typedef int16_t             Integer16;
    typedef int32_t             Integer32;
    typedef int64_t             Integer64;

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  ostream&              operator<<(ostream&,
                                   const elle::core::Integer8&);

}

#endif
