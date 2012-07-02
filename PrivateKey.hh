#ifndef ELLE_CRYPTOGRAPHY_PRIVATEKEY_HH
# define ELLE_CRYPTOGRAPHY_PRIVATEKEY_HH

# include <elle/types.hh>

# include <elle/radix/Object.hh>

# include <elle/cryptography/fwd.hh>
# include <elle/cryptography/Plain.hh>

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
  namespace cryptography
  {

    ///
    /// this class represents a private key based on the RSA cryptosystem.
    ///
    /// note that the cryptographic methods are set as virtual because
    /// some classes may wish to override it, as it is the case in the
    /// Etoile's agent.
    ///
    class PrivateKey:
      public radix::Object
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

    public:
      ///
      /// this method decrypts a code which should actually be
      /// an archive containing both a secret key and some data.
      ///
      /// this method starts by (i) extracting the key and data
      /// in their encrypted forms (ii) decrypt the symmetric key
      /// with the private key and (iii) decipher the data with the
      /// symmetric key.
      ///
      Status
      Decrypt(Code const& in, elle::utility::Buffer& out) const;
      template<typename T> Status
      Decrypt(Code const& in, T& out) const;

    public:
      ///
      /// This methods encrypt the given data with the private key.
      ///
      /// although unusual, the private key can very well be used for
      /// encrypting in which case the public key would be used for
      /// decrypting.
      ///
      /// since (i) the private key size limits the size of the data that
      /// can be encrypted and (ii) raising large data to large exponent
      /// is very slow; the algorithm below consists in (i) generating
      /// a secret key, (ii) ciphering the plain text with this key,
      /// (iii) encrypting the secret key with the private key and finally
      /// (iv) returning an archive containing the asymetrically-encrypted
      /// secret key with the symmetrically-encrypted data.
      ///
      Status
      Encrypt(elle::utility::WeakBuffer const& in,  Code& out) const;
      template<typename T> Status
      Encrypt(T const& in, Code& out) const;

    public:
      Status
      Sign(elle::utility::WeakBuffer const& in, Signature& out) const;
      template<typename T> Status
      Sign(T const& in, Signature& out) const;

    public:
      Status
      Derive(const Seed& seed, PublicKey& key) const;

      //
      // interfaces
      //

      // object
      declare(PrivateKey);
      Boolean           operator==(const PrivateKey&) const;

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
    private:
      ::EVP_PKEY*       _key;
      struct
      {
        ::EVP_PKEY_CTX* decrypt;
        ::EVP_PKEY_CTX* sign;
        ::EVP_PKEY_CTX* encrypt;
      }                 _contexts;

    public:
      ::EVP_PKEY const* key() const { return this->_key; }
    };

  }
}

#include <elle/cryptography/PrivateKey.hxx>

#endif
