//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       GPL
//
// file          /home/mycure/infinit/elle/crypto/KeyPair.hh
//
// created       julien quintard   [sat oct 27 18:00:55 2007]
// updated       julien quintard   [wed mar 11 16:02:14 2009]
//

#ifndef ELLE_CRYPTO_KEYPAIR_HH
#define ELLE_CRYPTO_KEYPAIR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/IO.hh>

#include <elle/core/Core.hh>

#include <elle/misc/Misc.hh>

#include <elle/archive/Archive.hh>

#include <elle/crypto/PublicKey.hh>
#include <elle/crypto/PrivateKey.hh>
#include <elle/crypto/Plain.hh>
#include <elle/crypto/Code.hh>
#include <elle/crypto/Clear.hh>
#include <elle/crypto/Signature.hh>

#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/evp.h>

#include <fcntl.h>

namespace elle
{
  using namespace io;
  using namespace core;
  using namespace misc;
  using namespace archive;

  namespace crypto
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a cryptographic key pair _i.e_ a pair of public
    /// and private keys.
    ///
    /// a public key is noted with a capital 'K' while a private key is
    /// noted with a lower-case 'k'.
    ///
    class KeyPair:
      public Object,
      public Dumpable, public Archivable
    {
    private:
      //
      // static methods
      //
      static Status	Initialize();

    public:
      //
      // constants
      //
      static const String		Class;

      struct Default
      {
	static const Natural32		Length;
      };

      //
      // constructors & destructors
      //
      KeyPair()
      {
	KeyPair::New(*this);
      }

      ~KeyPair()
      {
	KeyPair::Delete(*this);
      }

      //
      // methods
      //
      Status		Generate();
      Status		Generate(const Natural32);

      //
      // inline template-methods
      //
      template <typename T>
      inline Status	Encrypt(const T&			element,
				Code&				code) const
      {
	return (this->K.Encrypt(element, code));
      }

      template <typename T>
      inline Status	Decrypt(const Code&			code,
				T&				element) const
      {
	return (this->k.Decrypt(code, element));
      }

      template <typename T>
      inline Status	Sign(const T&				element,
			     Signature&				signature)
	const
      {
	return (this->k.Sign(element, signature));
      }

      template <typename T>
      inline Status	Verify(const Signature&			signature,
			       const T&				element) const
      {
	return (this->K.Verify(signature, element));
      }

      //
      // interfaces
      //

      // object
      static Status	New(KeyPair&);
      static Status	Delete(KeyPair&);

      KeyPair&		operator=(const KeyPair&);
      Boolean		operator==(const KeyPair&);
      Boolean		operator!=(const KeyPair&);

      // dumpable
      Status		Dump(const Natural32 = 0);

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      PublicKey		K;
      PrivateKey	k;

      struct			Contexts
      {
	static ::EVP_PKEY_CTX*	Generate;
      };
    };

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  std::ostream&		operator<<(std::ostream&,
				   const elle::crypto::KeyPair&);

}

#endif
