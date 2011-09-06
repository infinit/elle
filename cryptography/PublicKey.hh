//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/cryptography/PublicKey.hh
//
// created       julien quintard   [tue oct 30 01:08:16 2007]
// updated       julien quintard   [tue sep  6 16:26:14 2011]
//

#ifndef ELLE_CRYPTOGRAPHY_PUBLICKEY_HH
#define ELLE_CRYPTOGRAPHY_PUBLICKEY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Boolean.hh>
#include <elle/core/Natural.hh>
#include <elle/core/Large.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Object.hh>

#include <elle/package/Archive.hh>

#include <elle/cryptography/Plain.hh>
#include <elle/cryptography/Code.hh>
#include <elle/cryptography/Clear.hh>
#include <elle/cryptography/Signature.hh>

#include <elle/idiom/Close.hh>
# include <openssl/rsa.h>
# include <openssl/bn.h>
# include <openssl/err.h>
# include <openssl/evp.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;
  using namespace package;

  namespace cryptography
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a public key.
    ///
    class PublicKey:
      public Object
    {
    public:
      //
      // constants
      //
      static const PublicKey		Null;

      //
      // constructors & destructors
      //
      PublicKey();
      PublicKey(const PublicKey&);
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
      Status		Decrypt(const Code&,
				Clear&) const;

      //
      // interfaces
      //

      // object
      declare(PublicKey);
      Boolean		operator==(const PublicKey&) const;

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
	::EVP_PKEY_CTX*	encrypt;
	::EVP_PKEY_CTX*	verify;
	::EVP_PKEY_CTX*	decrypt;
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
	return (this->Encrypt(Plain(archive.contents, archive.size),
			      code));
      }

      Status		Verify(const Signature&		signature,
			       const Archive&		archive) const
      {
	return (this->Verify(signature,
			     Plain(archive.contents, archive.size)));
      }

      Status		Decrypt(const Code&		code,
				Archive&		archive) const
      {
	Clear		clear;

	enter();

	// decrypt the code.
	if (this->Decrypt(code, clear) == StatusError)
	  escape("unable to decrypt the code");

	// prepare the archive.
	if (archive.Acquire(clear) == StatusError)
	  escape("unable to prepare the archive");

	// detach the data so that not both the clear and archive
	// release the data.
	if (clear.Detach() == StatusError)
	  escape("unable to detach the clear's data");

	leave();
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
      template <typename T,
		typename... TT>
      Status		Verify(const Signature&,
			       const T&,
			       const TT&...) const;

      // decrypt
      template <typename T,
		typename... TT>
      Status		Decrypt(const Code&,
				T&,
				TT&...) const;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/cryptography/PublicKey.hxx>

#endif
