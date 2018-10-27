#pragma once

#include <utility>

#include <boost/operators.hpp>

#include <elle/attribute.hh>
#include <elle/operator.hh>
#include <elle/serialization/Serializer.hh>
#include <elle/serialization.hh>
#include <elle/cryptography/dsa/PrivateKey.hh>
#include <elle/cryptography/fwd.hh>
#include <elle/cryptography/types.hh>
#include <elle/cryptography/Oneway.hh>
#include <elle/cryptography/Cipher.hh>

//
// ---------- Class -----------------------------------------------------------
//

namespace elle
{
  namespace cryptography
  {
    namespace dsa
    {
      /// Represent a public key in the DSA asymmetric cryptosystem.
      class PublicKey
        : public elle::Printable
        , private boost::totally_ordered<PublicKey>
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
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
        /// Check that the key is valid.
        void
        _check() const;
      public:
        /// Return true if the given signature matches with the plain text.
        bool
        verify(elle::ConstWeakBuffer const& signature,
               elle::ConstWeakBuffer const& plain) const;
        /// Verify a signature against a stream-based plain text.
        bool
        verify(elle::ConstWeakBuffer const& signature,
               std::istream& plain) const;
        /// Return the public key's size in bytes.
        uint32_t
        size() const;
        /// Return the public key's length in bits.
        uint32_t
        length() const;

        /*----------.
        | Operators |
        `----------*/
      public:
        bool
        operator ==(PublicKey const& other) const;
        ELLE_OPERATOR_NO_ASSIGNMENT(PublicKey);

        /*----------.
        | Printable |
        `----------*/
      public:
        void
        print(std::ostream& stream) const override;

        /*--------------.
        | Serialization |
        `--------------*/
      public:
        PublicKey(elle::serialization::SerializerIn& serializer);
        void
        serialize(elle::serialization::Serializer& serializer);
        using serialization_tag = elle::serialization_tag;

        /*-----------.
        | Attributes |
        `-----------*/
      public:
        ELLE_ATTRIBUTE_R(types::EVP_PKEY, key);
        ELLE_ATTRIBUTE_R(Oneway, digest_algorithm);
      };
    }
  }
}

namespace std
{
  template <>
  struct hash<elle::cryptography::dsa::PublicKey>
  {
    size_t
    operator ()(elle::cryptography::dsa::PublicKey const& value) const;
  };
}
