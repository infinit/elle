#ifndef INFINIT_CRYPTOGRAPHY_DSA_PRIVATEKEY_HH
# define INFINIT_CRYPTOGRAPHY_DSA_PRIVATEKEY_HH

# include <cryptography/fwd.hh>
# include <cryptography/types.hh>
# include <cryptography/Plain.hh>
# include <cryptography/Clear.hh>
# include <cryptography/Signature.hh>
# include <cryptography/Oneway.hh>
# include <cryptography/Cipher.hh>

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
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
      /// Represent a private key in the DSA asymmetric cryptosystem.
      class PrivateKey:
        public elle::Printable
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
        PrivateKey(); // XXX[deserialize]
        /// Construct a private key based on the given EVP_PKEY key whose
        /// ownership is transferred.
        explicit
        PrivateKey(::EVP_PKEY* key,
                   Oneway const digest_algorithm);
        /// Construct a private key based on the given DSA key whose
        /// ownership is transferred to the private key.
        explicit
        PrivateKey(::DSA* dsa,
                   Oneway const digest_algorithm);
        PrivateKey(PrivateKey const& other);
        PrivateKey(PrivateKey&& other);
        ELLE_SERIALIZE_CONSTRUCT_DECLARE(PrivateKey);
        virtual
        ~PrivateKey() = default;

        /*--------.
        | Methods |
        `--------*/
      private:
        /// Construct the object based on the given DSA structure whose
        /// ownership is transferred to the callee.
        void
        _construct(::DSA* dsa);
        /// Prepare the private key cryptographic contexts.
        void
        _prepare();
        /// Check that the key is valid.
        void
        _check() const;
      public:
        /// Return a signature of the given digest.
        Signature
        sign(Digest const& digest) const;
        /// Return a signature of any given serializable type, including Plain.
        template <typename T = Plain>
        Signature
        sign(T const& value) const;
        /// Return the private key's size in bytes.
        elle::Natural32
        size() const;
        /// Return the private key's length in bits.
        elle::Natural32
        length() const;

        /*----------.
        | Operators |
        `----------*/
      public:
        elle::Boolean
        operator ==(PrivateKey const& other) const;
        ELLE_OPERATOR_NO_ASSIGNMENT(PrivateKey);

        /*----------.
        | Printable |
        `----------*/
      public:
        void
        print(std::ostream& stream) const override;

        /*----------.
        | Serialize |
        `----------*/
      public:
        ELLE_SERIALIZE_FRIEND_FOR(PrivateKey);

        /*-------------.
        | Serializable |
        `-------------*/
      public:
        PrivateKey(elle::serialization::SerializerIn& serializer);
        void
        serialize(elle::serialization::Serializer& serializer);

        /*-----------.
        | Attributes |
        `-----------*/
      private:
        ELLE_ATTRIBUTE_R(types::EVP_PKEY, key);
        ELLE_ATTRIBUTE_R(Oneway, digest_algorithm);
        // Note that the contexts are not serialized because they can
        // be reconstructed out of the paddings and algorithms above.
        struct
        {
          types::EVP_PKEY_CTX sign;
        } _context;
      };
    }
  }
}

# include <cryptography/dsa/PrivateKey.hxx>

#endif
