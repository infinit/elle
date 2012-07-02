#ifndef ELLE_CRYPTOGRAPHY_SECRETKEY_HH
# define ELLE_CRYPTOGRAPHY_SECRETKEY_HH

# include <elle/types.hh>

# include <elle/radix/Object.hh>

# include <elle/cryptography/fwd.hh>

# include <elle/idiom/Close.hh>
#  include <openssl/evp.h>
#  include <openssl/err.h>
#  include <openssl/rand.h>
#  include <fcntl.h>
# include <elle/idiom/Open.hh>

namespace elle
{
  namespace cryptography
  {

    ///
    /// this class represents a secret key for symmetric encryption.
    ///
    class SecretKey:
      public radix::Object
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

      Status
        Encrypt(elle::utility::WeakBuffer const& in, Cipher& out) const;
      template<typename T>
        Status Encrypt(T const& in, Cipher& out) const;


      Status
        Decrypt(Cipher const& in, elle::utility::Buffer& out) const;
      template<typename T>
        Status Decrypt(Cipher const& in, T& out) const;

      //
      // interfaces
      //

      // object
      declare(SecretKey);
      Boolean           operator==(const SecretKey&) const;

      // dumpable
      Status            Dump(const Natural32 = 0) const;

    public:
      standalone::Region region;
    };

  }
}

#include <elle/cryptography/SecretKey.hxx>

#endif
