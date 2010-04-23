//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/core/Large.hh
//
// created       julien quintard   [thu nov 29 20:38:10 2007]
// updated       julien quintard   [wed mar 10 20:39:13 2010]
//

#ifndef ELLE_CORE_LARGE_HH
#define ELLE_CORE_LARGE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/idiom/Close.hh>
# include <openssl/bn.h>
# include <ostream>
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
