#pragma once

#include <memory>
#include <utility>

#include <boost/operators.hpp>

#include <elle/attribute.hh>
#include <elle/operator.hh>
#include <elle/serialization.hh>

#include <elle/cryptography/fwd.hh>
#include <elle/cryptography/types.hh>
#include <elle/cryptography/Oneway.hh>
#include <elle/cryptography/Cipher.hh>
#if defined(ELLE_CRYPTOGRAPHY_ROTATION)
# include <elle/cryptography/rsa/Seed.hh>
#endif
#include <elle/cryptography/rsa/Padding.hh>
#include <elle/cryptography/rsa/defaults.hh>

namespace elle
{
  namespace cryptography
  {
    namespace rsa
    {
      /// Represent a public key in the RSA asymmetric cryptosystem.
      class PublicKey
        : public elle::Printable
        , public std::enable_shared_from_this<PublicKey>
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
        PublicKey(::EVP_PKEY* key);
        /// Construct a public key based on the given RSA key whose
        /// ownership is transferred to the public key.
        explicit
        PublicKey(::RSA* rsa);
        PublicKey(PublicKey const& other);
        PublicKey(PublicKey&& other);
        virtual
        ~PublicKey() = default;

        /*--------.
        | Methods |
        `--------*/
      private:
        /// Check that the key is valid.
        void
        _check() const;
      public:
        /// Encrypt the plain text and return the ciphered text in an envelope.
        virtual
        elle::Buffer
        seal(elle::ConstWeakBuffer const& plain,
             Cipher const cipher = defaults::envelope_cipher,
             Mode const mode = defaults::envelope_mode) const;
        /// Encrypt the stream-based plain text and seal it in an envelope.
        void
        seal(std::istream& plain,
             std::ostream& code,
             Cipher const cipher = defaults::envelope_cipher,
             Mode const mode = defaults::envelope_mode) const;
        /// Encrypt a plain text using the raw public key.
        ///
        /// WARNING: This method cannot be used to encrypt large amount of
        ///          data as constrained by the key's modulus. Please refer
        ///          to the seal()/open() methods.
        virtual
        elle::Buffer
        encrypt(elle::ConstWeakBuffer const& plain,
                Padding const padding = defaults::encryption_padding) const;
        /// Verify the given signature against the original plain text.
        bool
        verify(elle::ConstWeakBuffer const& signature,
               elle::ConstWeakBuffer const& plain,
               Padding const padding = defaults::signature_padding,
               Oneway const oneway = defaults::oneway) const;
        /// Verify the given signature against the object
        template <typename T>
        bool
        verify(elle::ConstWeakBuffer const& signature, T const& o) const;
        template <typename T>
        std::function<bool ()>
        verify_async(elle::ConstWeakBuffer const& signature, T const& o) const;
      private:
        virtual
        bool
        _verify(elle::ConstWeakBuffer const& signature,
                elle::ConstWeakBuffer const& plain,
                Padding const padding = defaults::signature_padding,
                Oneway const oneway = defaults::oneway) const;
        template <typename T>
        std::pair<elle::Buffer, elle::Buffer>
        _verify_data(elle::ConstWeakBuffer const& signature,
                     T const& o) const;
      public:
        /// Whether the given signature matches the stream-based plain.
        bool
        verify(elle::ConstWeakBuffer const& signature,
               std::istream& plain,
               Padding const padding = defaults::signature_padding,
               Oneway const oneway = defaults::oneway) const;
        /// Return the public key's size in bytes.
        uint32_t
        size() const;
        /// Return the public key's length in bits.
        uint32_t
        length() const;
      private:
        bool
        _verify(elle::ConstWeakBuffer const& signature,
                std::istream& plain,
                Padding const padding = defaults::signature_padding,
                Oneway const oneway = defaults::oneway) const;

# if defined(ELLE_CRYPTOGRAPHY_ROTATION)
        /*---------.
        | Rotation |
        `---------*/
      public:
        /// Construct a public key based on a given seed i.e in a deterministic
        /// way.
        explicit
        PublicKey(Seed const& seed);
        /// Return the seed once unrotated by the public key.
        Seed
        unrotate(Seed const& seed) const;
# endif

        /*----------.
        | Operators |
        `----------*/
      public:
        bool
        operator ==(PublicKey const& other) const;
        bool
        operator <(PublicKey const& other) const;
        PublicKey&
        operator =(PublicKey&& other) = default;

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
      };

      namespace _details
      {
        void
        raise(std::string const& message);
        types::EVP_PKEY
        build_evp(::RSA* rsa);
      }
    }
  }
}

//
// ---------- DER -------------------------------------------------------------
//

namespace elle
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace publickey
      {
        namespace der
        {
          /*----------.
          | Functions |
          `----------*/

          /// Encode the public key in DER.
          elle::Buffer
          encode(PublicKey const& K);
          /// Decode the public key from a DER representation.
          PublicKey
          decode(elle::ConstWeakBuffer const& buffer);
        }
      }
    }
  }
}

namespace std
{
  template <>
  struct hash<elle::cryptography::rsa::PublicKey>
  {
    size_t
    operator ()(elle::cryptography::rsa::PublicKey const& value) const;
  };
}

#include <elle/cryptography/rsa/PublicKey.hxx>
