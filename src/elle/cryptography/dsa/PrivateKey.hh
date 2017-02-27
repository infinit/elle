#ifndef ELLE_CRYPTOGRAPHY_DSA_PRIVATEKEY_HH
# define ELLE_CRYPTOGRAPHY_DSA_PRIVATEKEY_HH

# include <utility>

# include <openssl/evp.h>

# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/serialization.hh>

ELLE_OPERATOR_RELATIONALS();

# include <elle/cryptography/fwd.hh>
# include <elle/cryptography/types.hh>
# include <elle/cryptography/Oneway.hh>
# include <elle/cryptography/Cipher.hh>

//
// ---------- Class -----------------------------------------------------------
//

namespace elle
{
  namespace cryptography
  {
    namespace dsa
    {
      /// A private key in the DSA asymmetric cryptosystem.
      class PrivateKey:
        public elle::Printable
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
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
        /// Check that the key is valid.
        void
        _check() const;
      public:
        /// Return a signature of the given plain text.
        elle::Buffer
        sign(elle::ConstWeakBuffer const& plain) const;
        /// Sign a stream-based plain text.
        elle::Buffer
        sign(std::istream& plain) const;
        /// Return the private key's size in bytes.
        uint32_t
        size() const;
        /// Return the private key's length in bits.
        uint32_t
        length() const;

        /*----------.
        | Operators |
        `----------*/
      public:
        bool
        operator ==(PrivateKey const& other) const;
        ELLE_OPERATOR_NO_ASSIGNMENT(PrivateKey);

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
        PrivateKey(elle::serialization::SerializerIn& serializer);
        void
        serialize(elle::serialization::Serializer& serializer);
        typedef elle::serialization_tag serialization_tag;

        /*-----------.
        | Attributes |
        `-----------*/
      private:
        ELLE_ATTRIBUTE_R(types::EVP_PKEY, key);
        ELLE_ATTRIBUTE_R(Oneway, digest_algorithm);
      };
    }
  }
}

# include <elle/cryptography/dsa/PrivateKey.hxx>

#endif
