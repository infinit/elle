//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infi...libraries/elle/cryptography/SecretKey.hh
//
// created       julien quintard   [thu nov  1 11:38:37 2007]
// updated       julien quintard   [thu apr 22 14:32:18 2010]
//

#ifndef ELLE_CRYPTOGRAPHY_SECRETKEY_HH
#define ELLE_CRYPTOGRAPHY_SECRETKEY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/IO.hh>

#include <elle/core/Core.hh>

#include <elle/archive/Archive.hh>

#include <elle/miscellaneous/Status.hh>

#include <elle/cryptography/Plain.hh>
#include <elle/cryptography/Cipher.hh>
#include <elle/cryptography/Clear.hh>
#include <elle/cryptography/Digest.hh>
#include <elle/cryptography/OneWay.hh>

#include <elle/idiom/Close.hh>
# include <openssl/evp.h>
# include <openssl/err.h>
# include <openssl/rand.h>
# include <fcntl.h>
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
    /// this class represents a secret key for symmetric encryption.
    ///
    class SecretKey:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // constants
      //
      static const Character		Magic[];

      struct Default
      {
	static const Natural32		Length;
      };

      struct Algorithms
      {
	static const ::EVP_CIPHER*	Cipher;
	static const ::EVP_MD*		Digest;
      };

      static const SecretKey		Null;

      //
      // methods
      //
      Status		Create(const String&);

      Status		Generate();
      Status		Generate(const Natural32);

      Status		Encrypt(const Plain&,
				Cipher&) const;
      Status		Decrypt(const Cipher&,
				Clear&) const;

      template <typename T,
		typename... TT>
      Status		Decrypt(const Cipher&,
				T&,
				TT&...) const;

      //
      // interfaces
      //

      // entity
      declare(Entity, SecretKey);
      Boolean		operator==(const SecretKey&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      Region		region;

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

      Status		Encrypt(const Archive&		archive,
				Cipher&			cipher) const
      {
	return (this->Encrypt((Plain&)archive, cipher));
      }

      Status		Decrypt(const Cipher&		cipher,
				Archive&		archive) const
      {
	return (this->Decrypt(cipher, (Plain&)archive));
      }

      //
      // variadic templates
      //

      // encrypt
      template <typename T1>
      Status		Encrypt(const T1&,
				Cipher&) const;
      template <typename T1,
		typename T2>
      Status		Encrypt(const T1&,
				const T2&,
				Cipher&) const;
      template <typename T1,
		typename T2,
		typename T3>
      Status		Encrypt(const T1&,
				const T2&,
				const T3&,
				Cipher&) const;
      template <typename T1,
		typename T2,
		typename T3,
		typename T4>
      Status		Encrypt(const T1&,
				const T2&,
				const T3&,
				const T4&,
				Cipher&) const;
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
				Cipher&) const;
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
				Cipher&) const;
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
				Cipher&) const;
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
				Cipher&) const;
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
				Cipher&) const;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/cryptography/SecretKey.hxx>

#endif
