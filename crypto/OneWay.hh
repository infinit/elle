//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       GPL
//
// file          /home/mycure/infinit/elle/crypto/OneWay.hh
//
// created       julien quintard   [mon oct 29 13:19:49 2007]
// updated       julien quintard   [sat mar  7 23:14:06 2009]
//

#ifndef ELLE_CRYPTO_ONEWAY_HH
#define ELLE_CRYPTO_ONEWAY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

#include <elle/misc/Misc.hh>

#include <elle/crypto/Plain.hh>
#include <elle/crypto/Digest.hh>

#include <openssl/evp.h>
#include <openssl/err.h>

namespace elle
{
  using namespace core;
  using namespace misc;

  namespace crypto
  {

//
// ---------- class -----------------------------------------------------------
//

    ///
    /// this class provides one-way functions.
    ///
    class OneWay
    {
    public:
      //
      // constants
      //
      static const ::EVP_MD*		Algorithm;

      //
      // static methods
      //
      static Status	Hash(const Archivable&,
			     Digest&);
      static Status	Hash(const Plain&,
			     Digest&);
    };

  }
}

#endif
