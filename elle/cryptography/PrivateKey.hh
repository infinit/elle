//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue oct 30 10:02:18 2007]
//

#ifndef ELLE_CRYPTOGRAPHY_PRIVATEKEY_HH
#define ELLE_CRYPTOGRAPHY_PRIVATEKEY_HH

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
#include <elle/cryptography/Signature.hh>
#include <elle/cryptography/Clear.hh>
#include <elle/cryptography/Seed.hh>

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
  using namespace core;
  using namespace radix;
  using namespace package;

  namespace cryptography
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// the Seed must be forward declared to prevent conflicts.
    ///
    class Seed;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a private key based on the RSA cryptosystem.
    ///
    /// note that the cryptographic methods are set as virtual because
    /// some classes may wish to override it, as it is the case in the
    /// Etoile's agent.
    ///
    class PrivateKey:
      public Object
    {
    public:
      //
      // constants
      //
      static const PrivateKey           Null;

      //
      // constructors & destructors
      //
      PrivateKey();
      PrivateKey(const PrivateKey&);
      ~PrivateKey();

      //
      // methods
      //
      Status            Create(const ::EVP_PKEY*);
      Status            Create(Large*,
                               Large*,
                               Large*,
                               Large*,
                               Large*,
                               Large*,
                               Large*,
                               Large*);

      virtual Status    Decrypt(const Code&,
                                Clear&) const;
      virtual Status    Sign(const Plain&,
                             Signature&) const;
      virtual Status    Encrypt(const Plain&,
                                Code&) const;
      virtual Status    Derive(const Seed&,
                               PublicKey&) const;

      //
      // interfaces
      //

      // object
      declare(PrivateKey);
      Boolean           operator==(const PrivateKey&) const;

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      // archivable
      Status            Serialize(Archive&) const;
      Status            Extract(Archive&);

      //
      // attributes
      //
      ::EVP_PKEY*       key;

      struct
      {
        ::EVP_PKEY_CTX* decrypt;
        ::EVP_PKEY_CTX* sign;
        ::EVP_PKEY_CTX* encrypt;
      }                 contexts;

      //
      // forward methods
      //

      ///
      /// these methods basically handle the archive-specific cases.
      ///

      Status            Decrypt(const Code&                     code,
                                Archive&                        archive) const
      {
        Clear           clear;

        ;

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

        return elle::StatusOk;
      }

      Status            Sign(const Archive&             archive,
                             Signature&                 signature) const
      {
        return (this->Sign(Plain(archive.contents, archive.size),
                           signature));
      }

      Status            Encrypt(const Archive&          archive,
                                Code&                   code) const
      {
        return (this->Encrypt(Plain(archive.contents, archive.size),
                              code));
      }

      //
      // variadic templates
      //

      // decrypt
      template <typename T,
                typename... TT>
      Status            Decrypt(const Code&,
                                T&,
                                TT&...) const;

      // sign
      template <typename T1>
      Status            Sign(const T1&,
                             Signature&) const;
      template <typename T1,
                typename T2>
      Status            Sign(const T1&,
                             const T2&,
                             Signature&) const;
      template <typename T1,
                typename T2,
                typename T3>
      Status            Sign(const T1&,
                             const T2&,
                             const T3&,
                             Signature&) const;
      template <typename T1,
                typename T2,
                typename T3,
                typename T4>
      Status            Sign(const T1&,
                             const T2&,
                             const T3&,
                             const T4&,
                             Signature&) const;
      template <typename T1,
                typename T2,
                typename T3,
                typename T4,
                typename T5>
      Status            Sign(const T1&,
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
      Status            Sign(const T1&,
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
      Status            Sign(const T1&,
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
      Status            Sign(const T1&,
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
      Status            Sign(const T1&,
                             const T2&,
                             const T3&,
                             const T4&,
                             const T5&,
                             const T6&,
                             const T7&,
                             const T8&,
                             const T9&,
                             Signature&) const;

      // encrypt
      template <typename T1>
      Status            Encrypt(const T1&,
                                Code&) const;
      template <typename T1,
                typename T2>
      Status            Encrypt(const T1&,
                                const T2&,
                                Code&) const;
      template <typename T1,
                typename T2,
                typename T3>
      Status            Encrypt(const T1&,
                                const T2&,
                                const T3&,
                                Code&) const;
      template <typename T1,
                typename T2,
                typename T3,
                typename T4>
      Status            Encrypt(const T1&,
                                const T2&,
                                const T3&,
                                const T4&,
                                Code&) const;
      template <typename T1,
                typename T2,
                typename T3,
                typename T4,
                typename T5>
      Status            Encrypt(const T1&,
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
      Status            Encrypt(const T1&,
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
      Status            Encrypt(const T1&,
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
      Status            Encrypt(const T1&,
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
      Status            Encrypt(const T1&,
                                const T2&,
                                const T3&,
                                const T4&,
                                const T5&,
                                const T6&,
                                const T7&,
                                const T8&,
                                const T9&,
                                Code&) const;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/cryptography/PrivateKey.hxx>

#endif
