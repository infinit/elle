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
// updated       julien quintard   [sun mar  8 10:42:00 2009]
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

      //
      // static inline template-methods
      //
      template <typename T>
      static inline Status	Encrypt(const PublicKey&	K,
					const T&		element,
					Code&			code)
      {
	return (K.Encrypt(element, code));
      }

      template <typename T>
      static inline Status	Decrypt(const PrivateKey&	k,
					const Code&		code,
					T&			element)
      {
	return (k.Decrypt(code, element));
      }

      template <typename T>
      static inline Status	Sign(const PrivateKey&		k,
				     const T&			element,
				     Signature&			signature)
      {
	return (k.Sign(element, signature));
      }

      template <typename T>
      static inline Status	Verify(const PublicKey&		K,
				       const Signature&		signature,
				       const T&			element)
      {
	return (K.Verify(signature, element));
      }

      template <typename T>
      static inline Status	Encrypt(const KeyPair&		pair,
					const T&		element,
					Code&			code)
      {
	return (pair.Encrypt(element, code));
      }

      template <typename T>
      static inline Status	Decrypt(const KeyPair&		pair,
					const Code&		code,
					T&			element)
      {
	return (pair.Decrypt(code, element));
      }

      template <typename T>
      static inline Status	Sign(const KeyPair&		pair,
				     const T&			element,
				     Signature&			signature)
      {
	return (pair.Sign(element, signature));
      }

      template <typename T>
      static inline Status	Verify(const KeyPair&		pair,
				       const Signature&		signature,
				       const T&			element)
      {
	return (pair.Verify(signature, element));
      }

      template <typename T>
      static inline Status	Hash(const T&			element,
				     Digest&			digest)
      {
	return (OneWay::Hash(element, digest));
      }

      template <typename T>
      static inline Status	Encrypt(const SecretKey&	key,
					const T&		element,
					Code&			cipher)
      {
	return (key.Encrypt(element, cipher));
      }

      template <typename T>
      static inline Status	Decrypt(const SecretKey&	key,
					const Cipher&		cipher,
					T&			element)
      {
	return (key.Decrypt(cipher, element));
      }
    };

  }
}

#endif
