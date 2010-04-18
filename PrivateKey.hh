//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/cryptography/PrivateKey.hh
//
// created       julien quintard   [tue oct 30 10:02:18 2007]
// updated       julien quintard   [sun apr 18 12:36:48 2010]
//

#ifndef ELLE_CRYPTOGRAPHY_PRIVATEKEY_HH
#define ELLE_CRYPTOGRAPHY_PRIVATEKEY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/IO.hh>

#include <elle/core/Core.hh>

#include <elle/archive/Archive.hh>

#include <elle/miscellaneous/Status.hh>

#include <elle/cryptography/Plain.hh>
#include <elle/cryptography/Code.hh>
#include <elle/cryptography/Clear.hh>
#include <elle/cryptography/Signature.hh>
#include <elle/cryptography/Digest.hh>
#include <elle/cryptography/OneWay.hh>
#include <elle/cryptography/SecretKey.hh>

#include <elle/idiom/Close.hh>
# include <openssl/rsa.h>
# include <openssl/engine.h>
# include <openssl/bn.h>
# include <openssl/crypto.h>
# include <openssl/err.h>
# include <openssl/evp.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace io;
  using namespace core;
  using namespace miscellaneous;
  using namespace archive;

  namespace cryptography
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
      // constants
      //
      static const PrivateKey		Null;

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
			       Large*,
			       Large*,
			       Large*,
			       Large*,
			       Large*);

      Status		Decrypt(const Code&,
				Clear&) const;
      Status		Sign(const Plain&,
			     Signature&) const;

      template <typename T>
      Status		Decrypt(const Code&,
				T&) const;
      template <typename T,
		typename... TT>
      Status		Decrypt(const Code&,
				T&,
				TT&...) const;

      //
      // interfaces
      //

      // entity
      declare(Entity, PrivateKey);
      Boolean		operator==(const PrivateKey&) const;

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

#include <elle/cryptography/PrivateKey.hxx>

#endif
