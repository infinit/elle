#ifndef INFINIT_CRYPTOGRAPHY_PRIVATEKEY_HH
# define INFINIT_CRYPTOGRAPHY_PRIVATEKEY_HH

# include <cryptography/fwd.hh>
# include <cryptography/Signature.hh>
# include <cryptography/oneway.hh>

# include <elle/types.hh>
# include <elle/operator.hh>
# include <elle/serialize/fwd.hh>
# include <elle/serialize/construct.hh>
# include <elle/concept/Uniquable.hh>

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
      public elle::concept::MakeUniquable<PrivateKey>,
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
      /// Construct a private key based on the given EVP_PKEY.
      ///
      /// Note that the EVP_PKEY internal numbers are duplicate. Thus, the
      /// call remains the owner of the given EVP_PKEY.
      PrivateKey(::EVP_PKEY const* key);
      PrivateKey(PrivateKey const& other);
      ELLE_SERIALIZE_CONSTRUCT_DECLARE(PrivateKey);
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
      /// Return a signature of the given plain text.
      Signature
      sign(Plain const& plain) const;
      /// Return a signature of any given serializable type.
      template <typename T>
      Signature
      sign(T const& value) const;
      /// Encrypt the given plain text with the private key.
      ///
      /// Although unusual, the private key can very well be used for
      /// encrypting in which case the public key would be used for
      /// decrypting.
      Code
      encrypt(Plain const& plain) const;
      /// Encrypt the given serializable type with the private key.
      template <typename T>
      Code
      encrypt(T const& value) const;
      /// XXX
      /* XXX
      elle::Status
      Derive(const Seed& seed, PublicKey& key) const;
      */
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

      /*----------.
      | Operators |
      `----------*/
    public:
      elle::Boolean
      operator ==(PrivateKey const& other) const;
      elle::Boolean
      operator <(PrivateKey const& other) const;
      ELLE_OPERATOR_NO_ASSIGNMENT(PrivateKey);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // printable
      void
      print(std::ostream& stream) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(PrivateKey);

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE(::EVP_PKEY*, key);
      ELLE_ATTRIBUTE(::EVP_PKEY_CTX*, context_decrypt);
      ELLE_ATTRIBUTE(::EVP_PKEY_CTX*, context_sign);
      ELLE_ATTRIBUTE(::EVP_PKEY_CTX*, context_encrypt;)
    };
  }
}

# include <cryptography/PrivateKey.hxx>

#endif
