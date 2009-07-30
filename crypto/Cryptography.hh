//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/crypto/Cryptography.hh
//
// created       julien quintard   [tue oct 30 12:15:12 2007]
// updated       julien quintard   [thu jul 30 13:22:18 2009]
//

#ifndef ELLE_CRYPTO_CRYPTOGRAPHY_HH
#define ELLE_CRYPTO_CRYPTOGRAPHY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/misc/Misc.hh>

#include <elle/crypto/Cipher.hh>
#include <elle/crypto/Clear.hh>
#include <elle/crypto/Code.hh>
#include <elle/crypto/Digest.hh>
#include <elle/crypto/KeyPair.hh>
#include <elle/crypto/OneWay.hh>
#include <elle/crypto/Plain.hh>
#include <elle/crypto/PrivateKey.hh>
#include <elle/crypto/PublicKey.hh>
#include <elle/crypto/SecretKey.hh>
#include <elle/crypto/Signature.hh>

#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/rand.h>

namespace elle
{
  using namespace core;
  using namespace misc;

  ///
  /// this namespace contains everything necessary for performing
  /// cryptographic operations.
  ///
  namespace crypto
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents the cryptography package and provides a
    /// Initialise() method for initialising the package's components.
    ///
    /// this class also provides a general interface for cryptographic
    /// operations.
    ///
    class Cryptography
    {
    public:
      //
      // static methods
      //
      static Status		Initialize();
      static Status		Clean();
    };

  }
}

#endif
