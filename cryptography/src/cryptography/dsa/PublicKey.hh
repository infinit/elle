#ifndef INFINIT_CRYPTOGRAPHY_DSA_PUBLICKEY_HH
# define INFINIT_CRYPTOGRAPHY_DSA_PUBLICKEY_HH

# include <cryptography/dsa/PrivateKey.hh>
# include <cryptography/fwd.hh>
# include <cryptography/types.hh>
# include <cryptography/Code.hh>
# include <cryptography/Clear.hh>
# include <cryptography/Oneway.hh>
# include <cryptography/Cipher.hh>

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
    namespace dsa
    {
      /// Represent a public key in the DSA asymmetric cryptosystem.
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
        PublicKey(::EVP_PKEY* key,
                  Oneway const digest_algorithm);
        /// Construct a public key based on the given DSA key whose
        /// ownership is transferred to the public key.
        explicit
        PublicKey(::DSA* dsa,
                  Oneway const digest_algorithm);
        PublicKey(PublicKey const& other);
        PublicKey(PublicKey&& other);
        ELLE_SERIALIZE_CONSTRUCT_DECLARE(PublicKey);
        virtual
        ~PublicKey() = default;

        /*--------.
        | Methods |
        `--------*/
      private:
        /// Construct the object based on the given DSA structure whose
        /// ownership is transferred to the callee.
        void
        _construct(::DSA* dsa);
        /// Prepare the public key cryptographic contexts.
        void
        _prepare();
        /// Check that the key is valid.
        void
        _check() const;
      public:
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
        ELLE_ATTRIBUTE_R(Oneway, digest_algorithm);
        // Note that the contexts are not serialized because they can
        // be reconstructed out of the paddings and algorithms above.
        struct
        {
          types::EVP_PKEY_CTX verify;
        } _context;
      };
    }
  }
}

# include <cryptography/dsa/PublicKey.hxx>

#endif
