#pragma once

#include <utility>

#include <boost/operators.hpp>

#include <elle/cryptography/fwd.hh>
#include <elle/cryptography/types.hh>
#include <elle/cryptography/SecretKey.hh>
#include <elle/cryptography/Oneway.hh>
#include <elle/cryptography/Cipher.hh>
#include <elle/cryptography/dh/fwd.hh>

#include <elle/attribute.hh>
#include <elle/operator.hh>


//
// ---------- Class -----------------------------------------------------------
//

namespace elle
{
  namespace cryptography
  {
    namespace dh
    {
      /// Represent a private key in the DH asymmetric cryptosystem.
      class PrivateKey
        : public elle::Printable
        , private boost::totally_ordered<PrivateKey>
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
        /// Construct a private key based on the given EVP_PKEY key whose
        /// ownership is transferred.
        explicit
        PrivateKey(::EVP_PKEY* key);
        /// Construct a private key based on the given DH key whose
        /// ownership is transferred to the private key.
        explicit
        PrivateKey(::DH* dh);
        PrivateKey(PrivateKey const& other);
        PrivateKey(PrivateKey&& other);
        virtual
        ~PrivateKey() = default;

        /*--------.
        | Methods |
        `--------*/
      private:
        /// Construct the object based on the given DH structure whose
        /// ownership is transferred to the callee.
        void
        _construct(::DH* dh);
        /// Check that the key is valid.
        void
        _check() const;
      public:
        /// Compute a shared session secret key based on the peer's public
        /// key.
        SecretKey
        agree(PublicKey const& peer_K) const;
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

namespace std
{
  template <>
  struct hash<elle::cryptography::dh::PrivateKey>
  {
    size_t
    operator ()(elle::cryptography::dh::PrivateKey const& value) const;
  };
}
