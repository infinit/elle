#ifndef ELLE_CRYPTOGRAPHY_SECRETKEY_HH
# define ELLE_CRYPTOGRAPHY_SECRETKEY_HH

# include <elle/types.hh>

# include <elle/radix/Object.hh>

# include <elle/cryptography/Plain.hh>
# include <elle/cryptography/Cipher.hh>
# include <elle/cryptography/Clear.hh>

// XXX use pimpl pattern and hide openssl headers
# include <elle/idiom/Close.hh>
#  include <openssl/evp.h>
#  include <openssl/err.h>
#  include <openssl/rand.h>
#  include <fcntl.h>
# include <elle/idiom/Open.hh>

namespace elle
{

  using namespace radix;

  namespace cryptography
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a secret key for symmetric encryption.
    ///
    class SecretKey:
      public Object
    {
    public:
      //
      // constants
      //
      static const Character            Magic[];

      struct Default
      {
        static const Natural32          Length;
      };

      struct Algorithms
      {
        static const ::EVP_CIPHER*      Cipher;
        static const ::EVP_MD*          Digest;
      };

      static const SecretKey            Null;

      //
      // constructors & destructors
      //
      SecretKey();

      //
      // methods
      //
      Status            Create(const String&);

      Status            Generate();
      Status            Generate(const Natural32);

      Status            Encrypt(const Plain&,
                                Cipher&) const;
      Status            Decrypt(const Cipher&,
                                Clear&) const;

      //
      // interfaces
      //

      // object
      declare(SecretKey);
      Boolean           operator==(const SecretKey&) const;

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      // archivable
      //Status            Serialize(Archive&) const;
      //Status            Extract(Archive&);

      //
      // attributes
      //
      Region            region;

      //
      // forward methods
      //

      ///
      /// these methods basically convert archive-based into region-based
      /// calls.
      ///

      //Status            Encrypt(const Archive&          archive,
      //                          Cipher&                 cipher) const
      //{
      //  return (this->Encrypt(Plain(archive.contents, archive.size),
      //                        cipher));
      //}

      //Status            Decrypt(const Cipher&           cipher,
      //                          Archive&                archive) const
      //{
      //  Clear           clear;

      //  // decrypt the cipher into the clear.
      //  if (this->Decrypt(cipher, clear) == Status::Error)
      //    escape("unable to decrypt the cipher");

      //  // give the clear to the archive.
      //  if (archive.Acquire(clear) == Status::Error)
      //    escape("unable to acquire the region");

      //  // detach the region so that it does not get released twice.
      //  if (clear.Detach() == Status::Error)
      //    escape("unable to detach the region");

      //  return Status::Ok;
      //}

      //
      // variadic templates
      //

      // encrypt
      template <typename T1>
      Status            Encrypt(const T1&,
                                Cipher&) const;
      template <typename T1,
                typename T2>
      Status            Encrypt(const T1&,
                                const T2&,
                                Cipher&) const;
      template <typename T1,
                typename T2,
                typename T3>
      Status            Encrypt(const T1&,
                                const T2&,
                                const T3&,
                                Cipher&) const;
      template <typename T1,
                typename T2,
                typename T3,
                typename T4>
      Status            Encrypt(const T1&,
                                const T2&,
                                const T3&,
                                const T4&,
                                Cipher&) const;
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
                                Cipher&) const;
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
                                Cipher&) const;
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
                                Cipher&) const;
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
      Status            Encrypt(const T1&,
                                const T2&,
                                const T3&,
                                const T4&,
                                const T5&,
                                const T6&,
                                const T7&,
                                const T8&,
                                const T9&,
                                Cipher&) const;

      // decrypt
      template <typename T,
                typename... TT>
      Status            Decrypt(const Cipher&,
                                T&,
                                TT&...) const;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/cryptography/SecretKey.hxx>

#endif
