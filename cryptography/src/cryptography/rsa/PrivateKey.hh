#ifndef INFINIT_CRYPTOGRAPHY_RSA_PRIVATEKEY_HH
# define INFINIT_CRYPTOGRAPHY_RSA_PRIVATEKEY_HH

# include <cryptography/fwd.hh>
# include <cryptography/types.hh>
# include <cryptography/Clear.hh>
# include <cryptography/Signature.hh>
# include <cryptography/oneway.hh>
# include <cryptography/PrivateKey.hh>

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/serialize/fwd.hh>
# include <elle/serialize/construct.hh>
# include <elle/serialize/DynamicFormat.hh>
# include <elle/concept/Uniquable.hh>

# include <openssl/evp.h>

# include <utility>
ELLE_OPERATOR_RELATIONALS();

//
// ---------- Class -----------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      /// Represent a private key in the RSA asymmetric cryptosystem.
      class PrivateKey:
        public cryptography::privatekey::Interface,
        public elle::serialize::SerializableMixin<PrivateKey>,
        public elle::serialize::DynamicFormat<PrivateKey>,
        public elle::concept::MakeUniquable<PrivateKey>
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
        PrivateKey(); // XXX[deserialize]
        /// Construct a private key based on the given EVP_PKEY key whose
        /// ownership is transferred.
        explicit
        PrivateKey(::EVP_PKEY* key);
        /// Construct a private key based on the given RSA key whose
        /// ownership is transferred to the private key.
        explicit
        PrivateKey(::RSA* rsa);
        /// Construct a private key by transferring ownership of some big
        /// numbers.
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

        /*--------.
        | Methods |
        `--------*/
      public:
        /// Construct the object based on the given RSA structure whose
        /// ownership is transferred to the callee.
        void
        _construct(::RSA* rsa);
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
        operator ==(cryptography::privatekey::Interface const& other) const;
        virtual
        elle::Boolean
        operator <(cryptography::privatekey::Interface const& other) const;
        virtual
        cryptography::privatekey::Interface*
        clone() const;
        virtual
        elle::Natural32
        size() const;
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
        sign(Digest const& digest) const;
# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        virtual
        cryptography::Seed
        derive(cryptography::Seed const& seed) const;
        virtual
        cryptography::Seed
        rotate(cryptography::Seed const& seed) const;
# endif
        // printable
        void
        print(std::ostream& stream) const;
        // serializable
        ELLE_SERIALIZE_FRIEND_FOR(PrivateKey);

        /*-------------.
        | Serializable |
        `-------------*/
      public:
        PrivateKey(elle::serialization::SerializerIn& serializer);
        void
        serialize(elle::serialization::Serializer& serializer) override;

        /*-----------.
        | Attributes |
        `-----------*/
      private:
        ELLE_ATTRIBUTE_R(types::EVP_PKEY, key);
        ELLE_ATTRIBUTE(types::EVP_PKEY_CTX, context_decrypt);
        ELLE_ATTRIBUTE(types::EVP_PKEY_CTX, context_sign);
# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        ELLE_ATTRIBUTE(types::EVP_PKEY_CTX, context_derive);
        ELLE_ATTRIBUTE(types::EVP_PKEY_CTX, context_rotate);
# endif
      };
    }
  }
}

//
// ---------- Generator -------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace privatekey
      {
        /*----------.
        | Functions |
        `----------*/

# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        /// Generate a private key in a deterministic way (i.e deduce) based on
        /// the given seed.
        PrivateKey
        deduce(cryptography::seed::Interface const& seed);
# endif
        /// Construct a private key based on the given EVP structure whose
        /// ownership is transferred.
        PrivateKey
        construct(::EVP_PKEY* key);
      }
    }
  }
}

# include <cryptography/rsa/PrivateKey.hxx>

#endif
