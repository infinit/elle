#ifndef INFINIT_CRYPTOGRAPHY_RSA_PUBLICKEY_HH
# define INFINIT_CRYPTOGRAPHY_RSA_PUBLICKEY_HH

# include <cryptography/fwd.hh>
# include <cryptography/types.hh>
# include <cryptography/Code.hh>
# include <cryptography/Clear.hh>
# include <cryptography/PublicKey.hh>
# include <cryptography/Seed.hh>

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/serialize/fwd.hh>
# include <elle/serialize/construct.hh>
# include <elle/serialize/DynamicFormat.hh>
# include <elle/concept/Uniquable.hh>

# include <utility>
ELLE_OPERATOR_RELATIONALS();

# include <openssl/evp.h>

//
// ---------- Class -----------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      /// Represent a public key in the RSA asymmetric cryptosystem.
      class PublicKey:
        public cryptography::publickey::Interface,
        public elle::serialize::SerializableMixin<PublicKey>,
        public elle::serialize::DynamicFormat<PublicKey>,
        public elle::concept::MakeUniquable<PublicKey>
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
        PublicKey(); // XXX[to deserialize]
        /// Construct a public key based on the given EVP_PKEY key whose
        /// ownership is transferred.
        explicit
        PublicKey(::EVP_PKEY* key);
        /// Construct a public key based on the given RSA key whose
        /// ownership is transferred to the public key.
        explicit
        PublicKey(::RSA* rsa);
        /// Construct a public key by transferring ownership of some big
        /// numbers.
        PublicKey(::BIGNUM* n,
                  ::BIGNUM* e);
        PublicKey(PublicKey const& other);
        PublicKey(PublicKey&& other);
        ELLE_SERIALIZE_CONSTRUCT_DECLARE(PublicKey);

        /*--------.
        | Methods |
        `--------*/
      private:
        /// Construct the object based on the given RSA structure whose
        /// ownership is transferred to the callee.
        void
        _construct(::RSA* rsa);
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
        // publickey
        virtual
        elle::Boolean
        operator ==(cryptography::publickey::Interface const& other) const;
        virtual
        elle::Boolean
        operator <(cryptography::publickey::Interface const& other) const;
        virtual
        cryptography::publickey::Interface*
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
        Code
        encrypt(Plain const& plain) const;
        virtual
        elle::Boolean
        verify(Signature const& signature,
               Digest const& digest) const;
# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        virtual
        cryptography::Seed
        rotate(cryptography::Seed const& seed) const;
        virtual
        cryptography::Seed
        derive(cryptography::Seed const& seed) const;
# endif
        // printable
        virtual
        void
        print(std::ostream& stream) const;
        // serializable
        ELLE_SERIALIZE_FRIEND_FOR(PublicKey);

      /*-------------.
      | Serializable |
      `-------------*/
      public:
        PublicKey(elle::serialization::SerializerIn& serializer);
        void
        serialize(elle::serialization::Serializer& serializer) override;

      /*-----------.
      | Attributes |
      `-----------*/
      public:
        ELLE_ATTRIBUTE_R(types::EVP_PKEY, key);
        ELLE_ATTRIBUTE(types::EVP_PKEY_CTX, context_encrypt);
        // The padding size expressed in bits.
        ELLE_ATTRIBUTE(elle::Natural32, context_encrypt_padding_size);
        ELLE_ATTRIBUTE(types::EVP_PKEY_CTX, context_verify);
# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        ELLE_ATTRIBUTE(types::EVP_PKEY_CTX, context_rotate);
        ELLE_ATTRIBUTE(types::EVP_PKEY_CTX, context_derive);
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
      namespace publickey
      {
        /*----------.
        | Functions |
        `----------*/

# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        /// Generate a public key in a deterministic way (i.e deduce) based on
        /// the given seed.
        PublicKey
        deduce(cryptography::seed::Interface const& seed);
# endif
        /// Construct a public key based on the given EVP structure whose
        /// ownership is transferred.
        PublicKey
        construct(::EVP_PKEY* key);
      }
    }
  }
}

# include <cryptography/rsa/PublicKey.hxx>

#endif
