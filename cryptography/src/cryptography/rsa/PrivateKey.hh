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
    namespace rsa
    {
      /// Represent a private key in the RSA asymmetric cryptosystem.
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
# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        /// Construct a private key based on a given seed i.e in a deterministic
        /// way.
        explicit
        PrivateKey(Seed const& seed,
                   Padding const encryption_padding = Padding::oaep,
                   Padding const signature_padding = Padding::pss,
                   Oneway const digest_algorithm = Oneway::sha256);
# endif
        PrivateKey(PrivateKey const& other);
        PrivateKey(PrivateKey&& other);
        ELLE_SERIALIZE_CONSTRUCT_DECLARE(PrivateKey);
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
      public:
        /// Decrypt a code and returns the original clear text.
        ///
        /// Note that the code is, in practice, an archive containing both
        /// a temporarily-generated secret key and the plain text encrypted
        /// with the secret key.
        Clear
        decrypt(Code const& code) const;
        /// Decrypt a code and returns the given type, assuming the given type
        /// can be extracted from the clear, which should then be an archive.
        template <typename T>
        T
        decrypt(Code const& code) const;
        /// Return a signature of the given digest.
        Signature
        sign(Digest const& digest) const;
        /// Return a signature of the given plain text.
        Signature
        sign(Plain const& plain) const;
        /// Return a signature of any given serializable type.
        template <typename T>
        Signature
        sign(T const& value) const;
# if defined(INFINIT_CRYPTOGRAPHY_ROTATION)
        /// Return the seed once derived by the private key.
        Seed
        derive(Seed const& seed) const;
        /// Return the seed once rotated by the private key.
        Seed
        rotate(Seed const& seed) const;
# endif
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
          types::EVP_PKEY_CTX derive;
          types::EVP_PKEY_CTX rotate;
# endif
        } _context;
      };
    }
  }
}

# include <cryptography/rsa/PrivateKey.hxx>

#endif
