//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/crypto/PublicKey.hh
//
// created       julien quintard   [tue oct 30 01:08:16 2007]
// updated       julien quintard   [fri jul 31 01:07:08 2009]
//

#ifndef ELLE_CRYPTO_PUBLICKEY_HH
#define ELLE_CRYPTO_PUBLICKEY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/IO.hh>

#include <elle/core/Core.hh>

#include <elle/misc/Misc.hh>

#include <elle/archive/Archive.hh>

#include <elle/crypto/Plain.hh>
#include <elle/crypto/Code.hh>
#include <elle/crypto/Signature.hh>
#include <elle/crypto/Digest.hh>
#include <elle/crypto/OneWay.hh>
#include <elle/crypto/SecretKey.hh>

#include <openssl/rsa.h>
#include <openssl/bn.h>
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
    /// this class represents a public key.
    ///
    class PublicKey:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // constructors & destructors
      //
      PublicKey();
      ~PublicKey();

      //
      // methods
      //
      Status		Create(const ::EVP_PKEY*);
      Status		Create(Large*,
			       Large*);

      Status		Encrypt(const Plain&,
				Code&) const;
      Status		Verify(const Signature&,
			       const Plain&) const;

      //
      // interfaces
      //

      // entity
      PublicKey&	operator=(const PublicKey&);
      Boolean		operator==(const PublicKey&);
      Boolean		operator!=(const PublicKey&);

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
	::EVP_PKEY_CTX*	encrypt;
	::EVP_PKEY_CTX*	verify;
      }			contexts;

      ///
      /// this methods are required because the compiler, given an Archive
      /// object will call a template-based method instead of the Plain one.
      ///
      /// we do not want this especially because the template-based methods
      /// build archives and we are already receiving an archive.
      ///

      Status		Encrypt(const Archive&		archive,
				Code&			code) const
      {
	printf("[XXX] %s\n", __PRETTY_FUNCTION__);

	return (this->Encrypt((Plain&)archive, code));
      }

      Status		Verify(const Signature&		signature,
			       const Archive&		archive) const
      {
	printf("[XXX] %s\n", __PRETTY_FUNCTION__);

	return (this->Verify(signature, (Plain&)archive));
      }

      //
      // variadic templates
      //

      // encrypt
      template <typename T1>
      Status		Encrypt(const T1&,
				Code&) const;
      template <typename T1,
		typename T2>
      Status		Encrypt(const T1&,
				const T2&,
				Code&) const;
      template <typename T1,
		typename T2,
		typename T3>
      Status		Encrypt(const T1&,
				const T2&,
				const T3&,
				Code&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4>
      Status		Encrypt(const T1&,
				const T2&,
				const T3&,
				const T4&,
				Code&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5>
      Status		Encrypt(const T1&,
				const T2&,
				const T3&,
				const T4&,
				const T5&,
				Code&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6>
      Status		Encrypt(const T1&,
				const T2&,
				const T3&,
				const T4&,
				const T5&,
				const T6&,
				Code&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7>
      Status		Encrypt(const T1&,
				const T2&,
				const T3&,
				const T4&,
				const T5&,
				const T6&,
				const T7&,
				Code&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8>
      Status		Encrypt(const T1&,
				const T2&,
				const T3&,
				const T4&,
				const T5&,
				const T6&,
				const T7&,
				const T8&,
				Code&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8,
		typename T9>
      Status		Encrypt(const T1&,
				const T2&,
				const T3&,
				const T4&,
				const T5&,
				const T6&,
				const T7&,
				const T8&,
				const T9&,
				Code&) const;

      // verify
      template <typename T1>
      Status		Verify(const Signature&,
			       const T1&) const;
      template <typename T1,
		typename T2>
      Status		Verify(const Signature&,
			       const T1&,
			       const T2&) const;
      template <typename T1,
		typename T2,
		typename T3>
      Status		Verify(const Signature&,
			       const T1&,
			       const T2&,
			       const T3&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4>
      Status		Verify(const Signature&,
			       const T1&,
			       const T2&,
			       const T3&,
			       const T4&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5>
      Status		Verify(const Signature&,
			       const T1&,
			       const T2&,
			       const T3&,
			       const T4&,
			       const T5&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6>
      Status		Verify(const Signature&,
			       const T1&,
			       const T2&,
			       const T3&,
			       const T4&,
			       const T5&,
			       const T6&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7>
      Status		Verify(const Signature&,
			       const T1&,
			       const T2&,
			       const T3&,
			       const T4&,
			       const T5&,
			       const T6&,
			       const T7&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8>
      Status		Verify(const Signature&,
			       const T1&,
			       const T2&,
			       const T3&,
			       const T4&,
			       const T5&,
			       const T6&,
			       const T7&,
			       const T8&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4,
		typename T5,
		typename T6,
		typename T7,
		typename T8,
		typename T9>
      Status		Verify(const Signature&,
			       const T1&,
			       const T2&,
			       const T3&,
			       const T4&,
			       const T5&,
			       const T6&,
			       const T7&,
			       const T8&,
			       const T9&) const;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/crypto/PublicKey.hxx>

#endif
