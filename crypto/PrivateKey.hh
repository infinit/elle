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
// updated       julien quintard   [tue aug  4 13:53:27 2009]
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
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // constructors & destructors
      //
      PrivateKey();
      PrivateKey(const PrivateKey&);
      ~PrivateKey();

      //
      // methods
      //
      Status		Create(const ::EVP_PKEY*);
      Status		Create(Large*,
			       Large*,
			       Large*,
			       Large*);

      Status		Decrypt(const Code&,
				Clear&) const;
      Status		Sign(const Plain&,
			     Signature&) const;

      //
      // interfaces
      //

      // entity
      PrivateKey&	operator=(const PrivateKey&);
      Boolean		operator==(const PrivateKey&) const;
      Boolean		operator!=(const PrivateKey&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

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

      //
      // forward methods
      //

      ///
      /// this methods are required because the compiler, given an Archive
      /// object will call a template-based method instead of the Plain one.
      ///
      /// we do not want this especially because the template-based methods
      /// build archives and we are already receiving an archive.
      ///

      Status		Decrypt(const Code&		code,
				Archive&		archive) const
      {
	return (this->Decrypt(code, (Plain&)archive));
      }

      Status		Sign(const Archive&		archive,
			     Signature&			signature) const
      {
	return (this->Sign((Plain&)archive, signature));
      }

      //
      // variadic templates
      //

      // decrypt
      template <typename T1>
      Status		Decrypt(const Code&,
				T1&) const;
      template <typename T1,
		typename T2>
      Status		Decrypt(const Code&,
				T1&,
				T2&) const;
      template <typename T1,
		typename T2,
		typename T3>
      Status		Decrypt(const Code&,
				T1&,
				T2&,
				T3&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4>
      Status		Decrypt(const Code&,
				T1&,
				T2&,
				T3&,
				T4&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5>
      Status		Decrypt(const Code&,
				T1&,
				T2&,
				T3&,
				T4&,
				T5&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6>
      Status		Decrypt(const Code&,
				T1&,
				T2&,
				T3&,
				T4&,
				T5&,
				T6&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7>
      Status		Decrypt(const Code&,
				T1&,
				T2&,
				T3&,
				T4&,
				T5&,
				T6&,
				T7&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8>
      Status		Decrypt(const Code&,
				T1&,
				T2&,
				T3&,
				T4&,
				T5&,
				T6&,
				T7&,
				T8&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8,
		typename T9>
      Status		Decrypt(const Code&,
				T1&,
				T2&,
				T3&,
				T4&,
				T5&,
				T6&,
				T7&,
				T8&,
				T9&) const;

      // sign
      template <typename T1>
      Status		Sign(const T1&,
			     Signature&) const;
      template <typename T1,
		typename T2>
      Status		Sign(const T1&,
			     const T2&,
			     Signature&) const;
      template <typename T1,
		typename T2,
		typename T3>
      Status		Sign(const T1&,
			     const T2&,
			     const T3&,
			     Signature&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4>
      Status		Sign(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     Signature&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5>
      Status		Sign(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     Signature&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6>
      Status		Sign(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     Signature&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7>
      Status		Sign(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     const T7&,
			     Signature&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8>
      Status		Sign(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     const T7&,
			     const T8&,
			     Signature&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8,
		typename T9>
      Status		Sign(const T1&,
			     const T2&,
			     const T3&,
			     const T4&,
			     const T5&,
			     const T6&,
			     const T7&,
			     const T8&,
			     const T9&,
			     Signature&) const;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/crypto/PrivateKey.hxx>

#endif
