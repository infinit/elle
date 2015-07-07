#ifndef INFINIT_CRYPTOGRAPHY_RSA_PRIVATEKEY_HH
# define INFINIT_CRYPTOGRAPHY_RSA_PRIVATEKEY_HH

# include <cryptography/fwd.hh>
# include <cryptography/types.hh>
# include <cryptography/Plain.hh>
# include <cryptography/Clear.hh>
# include <cryptography/Signature.hh>
# include <cryptography/Oneway.hh>
# include <cryptography/Cipher.hh>
# include <cryptography/rsa/Seed.hh>
# include <cryptography/rsa/Padding.hh>
# include <cryptography/rsa/defaults.hh>

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>

# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
#  include <elle/serialize/construct.hh>
#  include <elle/concept/Uniquable.hh>
# endif

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
      /// Represent a private key in the RSA asymmetric cryptosystem.
      class PrivateKey:
        public elle::Printable
# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        , public elle::serialize::DynamicFormat<PrivateKey>
        , public elle::concept::MakeUniquable<PrivateKey>
# endif
      {
        /*-------------.
        | Construction |
        `-------------*/
      public:
        /// Construct a private key based on the given EVP_PKEY key whose
        /// ownership is transferred.
        explicit
        PrivateKey(::EVP_PKEY* key,
                   Padding const encryption_padding,
                   Padding const signature_padding,
                   Oneway const digest_algorithm);
        /// Construct a private key based on the given RSA key whose
        /// ownership is transferred to the private key.
        explicit
        PrivateKey(::RSA* rsa,
                   Padding const encryption_padding,
                   Padding const signature_padding,
                   Oneway const digest_algorithm);
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
        /// Prepare the private key cryptographic contexts.
        void
        _prepare();
        /// Check that the key is valid.
        void
        _check() const;
      public:
# if !defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        /// Decrypt and return the original plan text.
        Clear
        decrypt(Code const& code) const;
        /// Sign the given plain text.
        Signature
        sign(Plain const& plain) const;
# endif
        /// Return a signature of the given digest.
        Signature
        sign(Digest const& digest) const;
        /// Return the private key's size in bytes.
        elle::Natural32
        size() const;
        /// Return the private key's length in bits.
        elle::Natural32
        length() const;

# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        /*---------.
        | Rotation |
        `---------*/
      public:
        /// Construct a private key based on a given seed i.e in a deterministic
        /// way.
        explicit
        PrivateKey(Seed const& seed,
                   Padding const encryption_padding =
                     defaults::encryption_padding,
                   Padding const signature_padding =
                     defaults::signature_padding,
                   Oneway const digest_algorithm =
                     defaults::digest_algorithm);
        /// Return the seed once rotated by the private key.
        Seed
        rotate(Seed const& seed) const;
# endif

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
        ELLE_ATTRIBUTE_R(Padding, encryption_padding);
        ELLE_ATTRIBUTE_R(Padding, signature_padding);
        ELLE_ATTRIBUTE_R(Oneway, digest_algorithm);
        // Note that the contexts are not serialized because they can
        // be reconstructed out of the paddings and algorithms above.
        struct
        {
          types::EVP_PKEY_CTX decrypt;
          types::EVP_PKEY_CTX sign;
# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
          types::EVP_PKEY_CTX rotate;
# endif
        } _context;

# if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        /*-------.
        | Legacy |
        `-------*/
      public:
        // construction
        PrivateKey() {}
        ELLE_SERIALIZE_CONSTRUCT_DECLARE(PrivateKey)
        {}
        // methods
        template <typename T = Clear>
        T
        decrypt(Code const& code) const;
        template <typename T = Plain>
        Signature
        sign(T const& value) const;
        // serializable
        ELLE_SERIALIZE_FRIEND_FOR(PrivateKey);
        using elle::serialize::SerializableMixin<
          infinit::cryptography::rsa::PrivateKey,
          elle::serialize::Base64Archive>::serialize;
# endif

      };
    }
  }
}

# include <cryptography/rsa/PrivateKey.hxx>

#endif
