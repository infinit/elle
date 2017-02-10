#ifndef ELLE_CRYPTOGRAPHY_RSA_PRIVATEKEY_HH
# define ELLE_CRYPTOGRAPHY_RSA_PRIVATEKEY_HH

# include <memory>
# include <utility>

# include <openssl/evp.h>

# include <elle/serialization.hh>

ELLE_OPERATOR_RELATIONALS();

# include <cryptography/fwd.hh>
# include <cryptography/types.hh>
# include <cryptography/Oneway.hh>
# include <cryptography/Cipher.hh>
# include <cryptography/rsa/Seed.hh>
# include <cryptography/rsa/Padding.hh>
# include <cryptography/rsa/defaults.hh>

//
// ---------- Class -----------------------------------------------------------
//

namespace elle
{
  namespace cryptography
  {
    namespace rsa
    {
      /// Represent a private key in the RSA asymmetric cryptosystem.
      class PrivateKey
        : public elle::Printable
        , public std::enable_shared_from_this<PrivateKey>
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
        /// Construct a private key based on the given EVP_PKEY key whose
        /// ownership is transferred.
        explicit
        PrivateKey(::EVP_PKEY* key);
        /// Construct a private key based on the given RSA key whose
        /// ownership is transferred to the private key.
        explicit
        PrivateKey(::RSA* rsa);
        PrivateKey(PrivateKey const& other);
        PrivateKey(PrivateKey&& other);
        virtual
        ~PrivateKey() = default;

        /*--------.
        | Methods |
        `--------*/
      private:
        /// Construct the object based on the given RSA structure whose
        /// ownership is transferred to the callee.
        void
        _construct(::RSA* rsa);
        /// Check that the key is valid.
        void
        _check() const;
      public:
        /// Open the envelope and return the original plain text.
        elle::Buffer
        open(elle::ConstWeakBuffer const& code,
             Cipher const cipher = defaults::envelope_cipher,
             Mode const mode = defaults::envelope_mode) const;
        /// Open the envelope, decrypt its stream-based content and return the
        /// original plan text in the output stream.
        void
        open(std::istream& code,
             std::ostream& plain,
             Cipher const cipher = defaults::envelope_cipher,
             Mode const mode = defaults::envelope_mode) const;
        /// Decrypt a code with the raw public key.
        ///
        /// WARNING: This method cannot be used to decrypt large amount of
        ///          data as constrained by the key's modulus. Please refer
        ///          to the seal()/open() methods.
        elle::Buffer
        decrypt(elle::ConstWeakBuffer const& code,
                Padding const padding = defaults::encryption_padding) const;
        /// Sign the given plain text and return the signature.
        virtual
        elle::Buffer
        sign(elle::ConstWeakBuffer const& plain,
             Padding const padding = defaults::signature_padding,
             Oneway const oneway = defaults::oneway) const;
        /// Sign the given plain text and return the signature.
        template <typename T>
        elle::Buffer
        sign(T const& o) const;
        /// Sign the given plain text and return the signature.
        template <typename T>
        elle::Buffer
        sign(T const& o, elle::Version const& version) const;
        /// Sign the given plain text and return the signature.
        template <typename T>
        std::function <elle::Buffer ()>
        sign_async(T const& o, elle::Version const& version) const;
      private:
        template <typename T>
        std::function<elle::Buffer (PrivateKey const* self)>
        _sign_async(T const& o, elle::Version const& version) const;
      public:
        /// Write the signature in the output stream given the stream-based
        /// plain text.
        elle::Buffer
        sign(std::istream& plain,
             Padding const padding = defaults::signature_padding,
             Oneway const oneway = defaults::oneway) const;
        /// Return the private key's size in bytes.
        uint32_t
        size() const;
        /// Return the private key's length in bits.
        uint32_t
        length() const;

# if defined(ELLE_CRYPTOGRAPHY_ROTATION)
        /*---------.
        | Rotation |
        `---------*/
      public:
        /// Construct a private key based on a given seed i.e in a deterministic
        /// way.
        explicit
        PrivateKey(Seed const& seed);
        /// Return the seed once rotated by the private key.
        Seed
        rotate(Seed const& seed) const;
# endif

        /*----------.
        | Operators |
        `----------*/
      public:
        bool
        operator ==(PrivateKey const& other) const;
        PrivateKey&
        operator =(PrivateKey& other) = delete;
        PrivateKey&
        operator =(PrivateKey&& other);

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
      };
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
      namespace privatekey
      {
        namespace der
        {
          /*----------.
          | Functions |
          `----------*/

          /// Encode the private key in DER.
          elle::Buffer
          encode(PrivateKey const& K);
          /// Decode the private key from a DER representation.
          PrivateKey
          decode(elle::ConstWeakBuffer const& buffer);
        }
      }
    }
  }
}

# include <cryptography/rsa/PrivateKey.hxx>

#endif
