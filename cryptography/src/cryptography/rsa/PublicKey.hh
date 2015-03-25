#ifndef INFINIT_CRYPTOGRAPHY_RSA_PUBLICKEY_HH
# define INFINIT_CRYPTOGRAPHY_RSA_PUBLICKEY_HH

# include <cryptography/fwd.hh>
# include <cryptography/types.hh>
# include <cryptography/Code.hh>
# include <cryptography/Clear.hh>
# include <cryptography/rsa/Seed.hh>

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/serialize/fwd.hh>
# include <elle/serialize/construct.hh>

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
        public elle::Printable
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
        PublicKey(); // XXX[to deserialize]
        /// Construct a public key out of its private counterpart.
        explicit
        PublicKey(PrivateKey const& k);
        /// Construct a public key based on the given EVP_PKEY key whose
        /// ownership is transferred.
        explicit
        PublicKey(::EVP_PKEY* key);
        /// Construct a public key based on the given RSA key whose
        /// ownership is transferred to the public key.
        explicit
        PublicKey(::RSA* rsa);
# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        /// Construct a public key based on a given seed i.e in a deterministic
        /// way.
        explicit
        PublicKey(Seed const& seed);
# endif

        PublicKey(PublicKey const& other);
        PublicKey(PublicKey&& other);
        ELLE_SERIALIZE_CONSTRUCT_DECLARE(PublicKey);
        virtual
        ~PublicKey() = default;

        /*--------.
        | Methods |
        `--------*/
      private:
        /// Construct the object based on the given RSA structure whose
        /// ownership is transferred to the callee.
        void
        _construct(::RSA* rsa);
        /// Prepare the public key cryptographic contexts.
        void
        _prepare();
      public:
        /// Encrypt the given plain text.
        Code
        encrypt(Plain const& plain) const;
        /// Encrypt any serializable type.
        template <typename T>
        Code
        encrypt(T const& value) const;
        /// Return true if the given signature matches with the digest.
        elle::Boolean
        verify(Signature const& signature,
               Digest const& digest) const;
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
        ///
        /// Although unusual, the public key can very well be used for
        /// decrypting in which case the private key would be used for
        /// encrypting.
        Clear
        decrypt(Code const& code) const;
# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        /// Return the seed once rotated by the public key.
        Seed
        rotate(Seed const& seed) const;
        /// Return the seed once derived by the public key.
        Seed
        derive(Seed const& seed) const;
# endif
        /// Return the public key's size in bytes.
        elle::Natural32
        size() const;
        /// Return the public key's length in bits.
        elle::Natural32
        length() const;

        /*----------.
        | Operators |
        `----------*/
      public:
        elle::Boolean
        operator ==(PublicKey const& other) const;
        ELLE_OPERATOR_NO_ASSIGNMENT(PublicKey);

        /*----------.
        | Printable |
        `----------*/
      public:
        void
        print(std::ostream& stream) const override;

        /*-------------.
        | Serializable |
        `-------------*/
      public:
        ELLE_SERIALIZE_FRIEND_FOR(PublicKey);

        /*--------------.
        | Serialization |
        `--------------*/
      public:
        PublicKey(elle::serialization::SerializerIn& serializer);
        void
        serialize(elle::serialization::Serializer& serializer);

        /*-----------.
        | Attributes |
        `-----------*/
      public:
        ELLE_ATTRIBUTE_R(types::EVP_PKEY, key);
        ELLE_ATTRIBUTE(types::EVP_PKEY_CTX, context_encrypt);
        ELLE_ATTRIBUTE(types::EVP_PKEY_CTX, context_verify);
# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        ELLE_ATTRIBUTE(types::EVP_PKEY_CTX, context_rotate);
        ELLE_ATTRIBUTE(types::EVP_PKEY_CTX, context_derive);
# endif
        // The padding size expressed in bits.
        ELLE_ATTRIBUTE(elle::Natural32, context_encrypt_padding_size);
      };
    }
  }
}

# include <cryptography/rsa/PublicKey.hxx>

#endif
