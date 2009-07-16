//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/crypto/PrivateKey.hh
//
// created       julien quintard   [tue oct 30 10:02:18 2007]
// updated       julien quintard   [wed mar 11 15:54:17 2009]
//

#ifndef ELLE_CRYPTO_PRIVATEKEY_HH
#define ELLE_CRYPTO_PRIVATEKEY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/IO.hh>

#include <elle/core/Core.hh>

#include <elle/misc/Misc.hh>

#include <elle/archive/Archive.hh>

#include <elle/crypto/Plain.hh>
#include <elle/crypto/Code.hh>
#include <elle/crypto/Clear.hh>
#include <elle/crypto/Signature.hh>
#include <elle/crypto/Digest.hh>
#include <elle/crypto/OneWay.hh>
#include <elle/crypto/SecretKey.hh>

#include <openssl/rsa.h>
#include <openssl/engine.h>
#include <openssl/bn.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/evp.h>

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
    /// this class represents a private key based on the RSA cryptosystem.
    ///
    class PrivateKey:
      public Object,
      public Dumpable, public Archivable
    {
    public:
      //
      // constants
      //
      static const String		Class;

      //
      // constructors & destructors
      //
      PrivateKey()
      {
	PrivateKey::New(*this);
      }

      ~PrivateKey()
      {
	PrivateKey::Delete(*this);
      }

      //
      // methods
      //
      Status		Create(const ::EVP_PKEY*);
      Status		Create(Large*,
			       Large*,
			       Large*,
			       Large*);

      Status		Decrypt(const Code&,
				Archivable&) const;
      Status		Decrypt(const Code&,
				Clear&) const;

      Status		Sign(const Archivable&,
			     Signature&) const;
      Status		Sign(const Plain&,
			     Signature&) const;

      //
      // interfaces
      //

      // object
      static Status	New(PrivateKey&);
      static Status	Delete(PrivateKey&);

      PrivateKey&	operator=(const PrivateKey&);
      Boolean		operator==(const PrivateKey&);
      Boolean		operator!=(const PrivateKey&);

      // dumpable
      Status		Dump(const Natural32 = 0);

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      ::EVP_PKEY*	key;

      struct
      {
	::EVP_PKEY_CTX*	decrypt;
	::EVP_PKEY_CTX*	sign;
      }			contexts;
    };

  }
}

//
// ---------- operators -------------------------------------------------------
//

namespace std
{

  std::ostream&		operator<<(std::ostream&,
				   const elle::crypto::PrivateKey&);
}

#endif
