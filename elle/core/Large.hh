//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu nov 29 20:38:10 2007]
//

#ifndef ELLE_CORE_LARGE_HH
#define ELLE_CORE_LARGE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/idiom/Close.hh>
# include <ostream>
# include <openssl/bn.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  namespace core
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// this type wraps BIGNUMs.
    ///
    typedef ::BIGNUM    Large;

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  ostream&              operator<<(ostream&,
                                   const elle::core::Large&);

}

#endif
