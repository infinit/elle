//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/crypto/Plain.hh
//
// created       julien quintard   [wed feb 11 12:39:47 2009]
// updated       julien quintard   [sat mar  7 22:03:03 2009]
//

#ifndef ELLE_CRYPTO_PLAIN_HH
#define ELLE_CRYPTO_PLAIN_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Region.hh>

namespace elle
{
  using namespace misc;

  namespace crypto
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// this class represents plain data that can be used for cryptographic
    /// purposes.
    ///
    typedef Region	Plain;

  }
}

#endif
