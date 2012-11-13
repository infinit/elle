#ifndef INFINIT_CRYPTOGRAPHY_PRIVATEKEY_HH
# define INFINIT_CRYPTOGRAPHY_PRIVATEKEY_HH

# include <cryptography/fwd.hh>
# include <cryptography/Signature.hh>
# include <cryptography/oneway.hh>

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/io/Dumpable.hh>
# include <elle/serialize/fwd.hh>

# include <openssl/bn.h>
# include <openssl/evp.h>

# include <utility>
ELLE_OPERATOR_RELATIONALS();

namespace infinit
{
  namespace cryptography
  {
    /// Represent a private key in a given cryptosystem, RSA for instance.
    class PrivateKey:
      public elle::io::Dumpable,
      public elle::Printable
    {
      /*----------.
      | Constants |
      `----------*/
    public:
      struct Constants
      {
        static oneway::Algorithm const oneway_algorithm;
      };

      /*-------------.
      | Construction |
      `-------------*/
    public:
      PrivateKey(); // XXX[deserialize]
      PrivateKey(::EVP_PKEY const* key);
      PrivateKey(PrivateKey const& other);
      // XXX[move?]
      ~PrivateKey();
    private:
      PrivateKey(::BIGNUM* n,
                 ::BIGNUM* e,
                 ::BIGNUM* d,
                 ::BIGNUM* p,
                 ::BIGNUM* q,
                 ::BIGNUM* dmp1,
                 ::BIGNUM* dmq1,
                 ::BIGNUM* iqmp);

      /*--------.
      | Methods |
      `--------*/
    private:
      /// Construct the object based on big numbers.
      ///
      /// Note that when called, the number are already allocated for the
      /// purpose of the object construction. In other words, the ownership
      /// is transferred to the private key being constructed. Thus, it is
      /// the responsibility of the private key being constructed to release
      /// memory should an error occur, i.e not the caller's
      void
      _construct(::BIGNUM* n,
                 ::BIGNUM* e,
                 ::BIGNUM* d,
                 ::BIGNUM* p,
                 ::BIGNUM* q,
                 ::BIGNUM* dmp1,
                 ::BIGNUM* dmq1,
                 ::BIGNUM* iqmp);
    public:
      /// Decrypt a code and returns the original clear text.
      ///
      /// Note that the code is, in practice, an archive containing both
      /// a temporarily-generated secret key and the plain text encrypted
      /// with the secret key.
      ///
      Clear
      decrypt(Code const& code) const;
      /// Decrypt a code and returns the given type, assuming the given type
      /// can be extracted from the clear, which should then be an archive.
      template <typename T>
      T
      decrypt(Code const& code) const;

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

      /*----------.
      | Operators |
      `----------*/
    public:
      elle::Boolean
      operator ==(PrivateKey const& other) const;
      ELLE_OPERATOR_ASSIGNMENT(PrivateKey); // XXX

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status            Dump(const elle::Natural32 = 0) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(PrivateKey);
      // printable
      void
      print(std::ostream& stream) const;

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
      ::EVP_PKEY const* key() const { return this->_key; } // XXX
    };

  }
}

#include <cryptography/PrivateKey.hxx>

#endif
