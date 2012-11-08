#ifndef INFINIT_CRYPTOGRAPHY_PRIVATEKEY_HH
# define INFINIT_CRYPTOGRAPHY_PRIVATEKEY_HH

# include <cryptography/fwd.hh>
# include <cryptography/Plain.hh>
# include <cryptography/Signature.hh>
# include <cryptography/oneway.hh>

# include <elle/types.hh>
# include <elle/io/Dumpable.hh>
# include <elle/radix/Object.hh>

#include <elle/idiom/Close.hh>
# include <openssl/rsa.h>
# include <openssl/engine.h>
# include <openssl/bn.h>
# include <openssl/crypto.h>
# include <openssl/err.h>
# include <openssl/evp.h>
#include <elle/idiom/Open.hh>

namespace infinit
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
      public elle::radix::Object
    {
      /*----------.
      | Constants |
      `----------*/
    public:
      struct Constants
      {
        static oneway::Algorithm const oneway_algorithm;
      };

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
      elle::Status            Create(const ::EVP_PKEY*);
      elle::Status            Create(elle::Large*,
                               elle::Large*,
                               elle::Large*,
                               elle::Large*,
                               elle::Large*,
                               elle::Large*,
                               elle::Large*,
                               elle::Large*);

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
      elle::Status
      Decrypt(Code const& in, elle::Buffer& out) const;
      template<typename T> elle::Status
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
      elle::Status
      Encrypt(elle::WeakBuffer const& in,
              Code& out) const;
      template<typename T> elle::Status
      Encrypt(T const& in,
              Code& out) const;

    public:
      /// Returns a signature of the given buffer.
      Signature
      sign(elle::WeakBuffer const& plain) const;
      /// Returns a signature of any given type. Note that the given compound
      /// must be serializable.
      template <typename T>
      Signature
      sign(T const& plain) const;

    public:
      elle::Status
      Derive(const Seed& seed, PublicKey& key) const;

      //
      // interfaces
      //

      // object
      declare(PrivateKey);
      elle::Boolean           operator==(const PrivateKey&) const;

      // dumpable
      elle::Status            Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
    public: // XXX
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

#include <cryptography/PrivateKey.hxx>

#endif
