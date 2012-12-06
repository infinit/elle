#ifndef INFINIT_CRYPTOGRAPHY_SECRETKEY_HH
# define INFINIT_CRYPTOGRAPHY_SECRETKEY_HH

# include <elle/types.hh>

# include <elle/radix/Object.hh>

# include <elle/standalone/Region.hh>

# include <cryptography/fwd.hh>

# include <elle/serialize/Polymorphic.hh>

# include <elle/idiom/Close.hh>
#  include <openssl/evp.h>
#  include <openssl/err.h>
#  include <openssl/rand.h>
#  include <fcntl.h>
# include <elle/idiom/Open.hh>

namespace infinit
{
  namespace cryptography
  {
    ///
    /// this class represents a secret key for symmetric encryption.
    ///
    class SecretKey:
      public elle::radix::Object
    {
    public:
      //
      // constants
      //
      static const elle::Character            Magic[];

      struct Default
      {
        static const elle::Natural32          Length;
      };

      struct Algorithms
      {
        static const ::EVP_CIPHER*      Cipher;
        static const ::EVP_MD*          Digest;
      };

      static const SecretKey            Null;

      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// XXX
      static
      SecretKey
      generate(elle::Natural32 const length);

      //
      // constructors & destructors
      //
      SecretKey();

      //
      // methods
      //
      elle::Status            Create(const elle::String&);

      elle::Status            Generate();
      elle::Status            Generate(const elle::Natural32);

      elle::Status
      Encrypt(elle::WeakBuffer const& in,
              Cipher& out) const;
      template <typename T>
      elle::Status Encrypt(T const& in, Cipher& out) const;

      elle::Status
      Decrypt(Cipher const& in,
              elle::Buffer& out) const;
      template<typename T>
      elle::Status Decrypt(Cipher const& in, T& out) const;
      /* XXX
      /// Return an encrypted version of the given plain text.
      Cipher
      encrypt(Plain const& plain) const;
      /// Return an encrypted version of the given serializable value.
      template <typename T>
      Cipher
      encrypt(T&& value) const;
      /// Return a decrypted version of the given cipher text.
      Clear
      decrypt(Cipher const& cipher) const;
      /// Return a decrypted serializable value of the given cipher text.
      template <typename T>
      T
      decrypt(Cipher const& in, T& out) const; // XXX forward?
      */

      template<typename T>
        elle::Status Decrypt(Cipher const& in,
                       elle::serialize::Polymorphic<T> const& out) const
        {
          return this->Decrypt<elle::serialize::Polymorphic<T> const>(in, out);
        }

      //
      // interfaces
      //

      // object
      declare(SecretKey);
      elle::Boolean           operator==(const SecretKey&) const;

      // dumpable
      elle::Status            Dump(const elle::Natural32 = 0) const;

    public:
      elle::standalone::Region region;
    };

  }
}

#include <cryptography/SecretKey.hxx>

#endif
