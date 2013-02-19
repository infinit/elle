#ifndef INFINIT_CRYPTOGRAPHY_RSA_PRIVATEKEY_HH
# define INFINIT_CRYPTOGRAPHY_RSA_PRIVATEKEY_HH

# include <cryptography/fwd.hh>
# include <cryptography/Clear.hh>
# include <cryptography/Signature.hh>
# include <cryptography/oneway.hh>
# include <cryptography/PrivateKey.hh>

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/serialize/fwd.hh>
# include <elle/serialize/construct.hh>
# include <elle/concept/Uniquable.hh>

# include <openssl/evp.h>

# include <utility>
ELLE_OPERATOR_RELATIONALS();

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      /// Represent a private key in the RSA asymmetric cryptosystem.
      class PrivateKey:
        public privatekey::Interface,
        public elle::serialize::SerializableMixin<PrivateKey>,
        public elle::concept::MakeUniquable<PrivateKey>
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
        PrivateKey(); // XXX[deserialize]
        /// Construct a private key based on the given EVP_PKEY key whose
        /// ownership is transferred to the private key.
        explicit
        PrivateKey(::EVP_PKEY* key);
        /// Construct a private key by transferring ownership of some big
        /// numbers so as to build an EVP_PKEY.
        PrivateKey(::BIGNUM* n,
                   ::BIGNUM* e,
                   ::BIGNUM* d,
                   ::BIGNUM* p,
                   ::BIGNUM* q,
                   ::BIGNUM* dmp1,
                   ::BIGNUM* dmq1,
                   ::BIGNUM* iqmp);
        PrivateKey(PrivateKey const& other);
        PrivateKey(PrivateKey&& other);
        ELLE_SERIALIZE_CONSTRUCT_DECLARE(PrivateKey);
        ~PrivateKey();

        /*--------.
        | Methods |
        `--------*/
      public:
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
        /// Prepare the private key cryptographic contexts.
        void
        _prepare();

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
        // privatekey
        virtual
        elle::Boolean
        operator ==(privatekey::Interface const& other) const;
        virtual
        elle::Boolean
        operator <(privatekey::Interface const& other) const;
        virtual
        privatekey::Interface*
        clone() const;
        virtual
        elle::Natural32
        length() const;
        virtual
        Cryptosystem
        cryptosystem() const;
        virtual
        Clear
        decrypt(Code const& code) const;
        virtual
        Signature
        sign(Plain const& plain) const;
        virtual
        Code
        encrypt(Plain const& plain) const;
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
}

# include <cryptography/rsa/PrivateKey.hxx>

#endif
