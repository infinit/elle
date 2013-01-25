#ifndef INFINIT_CRYPTOGRAPHY_PUBLICKEY_HH
# define INFINIT_CRYPTOGRAPHY_PUBLICKEY_HH

# include <cryptography/Plain.hh>
# include <cryptography/Code.hh>
# include <cryptography/Clear.hh>
# include <cryptography/Signature.hh>
# include <cryptography/oneway.hh>

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/Printable.hh>
# include <elle/Buffer.hh>
# include <elle/serialize/construct.hh>
# include <elle/concept/Uniquable.hh>

# include <openssl/rsa.h>
# include <openssl/bn.h>
# include <openssl/err.h>
# include <openssl/evp.h>

# include <utility>
ELLE_OPERATOR_RELATIONALS();

namespace infinit
{
  namespace cryptography
  {
    /// Represent a public key in an asymmetric cryptosystem.
    class PublicKey:
      public elle::concept::MakeUniquable<PublicKey>,
      public elle::Printable
    {
      /*--------.
      | Friends |
      `--------*/
    public:
      friend class KeyPair;

      /*----------.
      | Constants |
      `----------*/
    public:
      struct Constants
      {
        static const oneway::Algorithm oneway_algorithm;
      };

      /*-------------.
      | Construction |
      `-------------*/
    public:
      PublicKey(); // XXX[to deserialize]
      PublicKey(PublicKey const& other);
      PublicKey(PublicKey&& other);
      ELLE_SERIALIZE_CONSTRUCT_DECLARE(PublicKey);
      ~PublicKey();
    private:
      /// Construct a public key based on the given EVP_PKEY key whose
      /// ownership is transferred to the public key.
      PublicKey(::EVP_PKEY* key);
      PublicKey(::BIGNUM* n,
                ::BIGNUM* e);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Encrypt the given plain text.
      Code
      encrypt(Plain const& plain) const;
      /// Encrypt any serializable type.
      template <typename T>
      Code
      encrypt(T const& value) const;
      /// Return true if the given signature matches with the plain text.
      elle::Boolean
      verify(Signature const& signature,
             Plain const& plain) const;
      /// Return true if the given signature matches with the serializable
      /// value.
      template <typename T>
      elle::Boolean
      verify(Signature const& signature,
             T const& value) const;
      /// Decrypt the given code.
      Clear
      decrypt(Code const& code) const;
      /// Decrypt any serializable type.
      template <typename T>
      T
      decrypt(Code const& code) const;
    private:
      /// Construct the object based on big numbers.
      ///
      /// Note that when called, the number are already allocated for the
      /// purpose of the object construction. In other words, the ownership
      /// is transferred to the public key being constructed. Thus, it is
      /// the responsibility of the public key being constructed to release
      /// memory should an error occur, i.e not the caller's
      void
      _construct(::BIGNUM* n,
                 ::BIGNUM* e);
      /// Prepare the public key cryptographic contexts.
      void
      _prepare();

      /*----------.
      | Operators |
      `----------*/
    public:
      elle::Boolean
      operator ==(PublicKey const& other) const;
      elle::Boolean
      operator <(PublicKey const& other) const;
      ELLE_OPERATOR_NO_ASSIGNMENT(PublicKey);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(PublicKey);

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE(::EVP_PKEY*, key);
      ELLE_ATTRIBUTE(::EVP_PKEY_CTX*, context_encrypt);
      ELLE_ATTRIBUTE(::EVP_PKEY_CTX*, context_verify);
      ELLE_ATTRIBUTE(::EVP_PKEY_CTX*, context_decrypt);
    };
  }
}

# include <cryptography/PublicKey.hxx>

#endif
