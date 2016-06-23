#ifndef INFINIT_CRYPTOGRAPHY_RSA_PUBLICKEY_HH
# define INFINIT_CRYPTOGRAPHY_RSA_PUBLICKEY_HH

# include <memory>
# include <utility>

# include <openssl/evp.h>

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/serialization.hh>

ELLE_OPERATOR_RELATIONALS();

# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
#  include <elle/serialize/fwd.hh>
#  include <elle/serialize/construct.hh>
#  include <elle/concept/Uniquable.hh>
# endif

# include <cryptography/fwd.hh>
# include <cryptography/types.hh>
# include <cryptography/Oneway.hh>
# include <cryptography/Cipher.hh>
# include <cryptography/rsa/Seed.hh>
# include <cryptography/rsa/Padding.hh>
# include <cryptography/rsa/defaults.hh>

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      /// Represent a public key in the RSA asymmetric cryptosystem.
      class PublicKey
        : public elle::Printable
        , public std::enable_shared_from_this<PublicKey>
# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        , public elle::serialize::DynamicFormat<PublicKey>
        , public elle::concept::MakeUniquable<PublicKey>
# endif
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
        PublicKey(::EVP_PKEY* key
# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
                  , Padding const encryption_padding =
                    defaults::encryption_padding
                  , Padding const signature_padding =
                    defaults::signature_padding
                  , Oneway const oneway =
                    defaults::oneway
                  , Cipher const envelope_cipher =
                    defaults::envelope_cipher
                  , Mode const envelope_mode =
                    defaults::envelope_mode
                  , uint16_t legacy_format =
                    elle::serialize::StaticFormat<PublicKey>::version
                  , uint16_t dynamic_format = 0
# endif
                 );
        /// Construct a public key based on the given RSA key whose
        /// ownership is transferred to the public key.
        explicit
        PublicKey(::RSA* rsa
# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
                  , Padding const encryption_padding =
                    defaults::encryption_padding
                  , Padding const signature_padding =
                    defaults::signature_padding
                  , Oneway const oneway =
                    defaults::oneway
                  , Cipher const envelope_cipher =
                    defaults::envelope_cipher
                  , Mode const envelope_mode =
                    defaults::envelope_mode
#endif
                 );
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
# if !defined(INFINIT_CRYPTOGRAPHY_LEGACY)
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
        elle::Buffer
        encrypt(elle::ConstWeakBuffer const& plain,
                Padding const padding = defaults::encryption_padding) const;
        /// Verify the given signature against the original plain text.
        virtual
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
        template <typename T>
        std::pair<elle::Buffer, elle::Buffer>
        _verify_data(elle::ConstWeakBuffer const& signature,
                     T const& o) const;

# endif
      public:
        /// Return true if the given signature matches the stream-based plain.
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

# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
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
        typedef elle::serialization_tag serialization_tag;

        /*-----------.
        | Attributes |
        `-----------*/
      public:
        ELLE_ATTRIBUTE_R(types::EVP_PKEY, key);

# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        /*-------.
        | Legacy |
        `-------*/
      public:
        // construction
        PublicKey() {}
        ELLE_SERIALIZE_CONSTRUCT(PublicKey)
        {}
        // methods
        template <typename T = Plain>
        Code
        encrypt(T const& value) const;
        template <typename T = Plain>
        bool
        verify(Signature const& signature, T const& value) const;
        // operators
        bool
        operator <(PublicKey const& other) const;
        // serializable
        ELLE_SERIALIZE_FRIEND_FOR(PublicKey);
        using elle::serialize::SerializableMixin<
          infinit::cryptography::rsa::PublicKey,
          elle::serialize::Base64Archive>::serialize;
        // attributes
        ELLE_ATTRIBUTE_R(Padding, encryption_padding);
        ELLE_ATTRIBUTE_R(Padding, signature_padding);
        ELLE_ATTRIBUTE_R(Oneway, oneway);
        ELLE_ATTRIBUTE_R(Cipher, envelope_cipher);
        ELLE_ATTRIBUTE_R(Mode, envelope_mode);
        ELLE_ATTRIBUTE_R(uint16_t, legacy_format);
# endif
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

namespace infinit
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

# include <cryptography/rsa/PublicKey.hxx>

#endif
