//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/core/Large.hh
//
// created       julien quintard   [thu nov 29 20:38:10 2007]
// updated       julien quintard   [wed feb 18 15:59:19 2009]
//

#ifndef ELLE_CORE_LARGE_HH
#define ELLE_CORE_LARGE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <openssl/bn.h>

#include <ostream>

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
    typedef ::BIGNUM	Large;

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{
  std::ostream&		operator<<(std::ostream&,
				   const elle::core::Large&);
}

#endif
