#ifndef ELLE_CRYPTOGRAPHY_RSA_KEYPAIR_HH
# define ELLE_CRYPTOGRAPHY_RSA_KEYPAIR_HH

# include <iosfwd>

# include <utility>

# include <elle/types.hh>
# include <elle/serialization.hh>
# include <elle/serialization/Serializer.hh>

# include <elle/cryptography/fwd.hh>
# include <elle/cryptography/Oneway.hh>
# include <elle/cryptography/Cipher.hh>
# include <elle/cryptography/rsa/KeyPair.hh>
# include <elle/cryptography/rsa/PublicKey.hh>
# include <elle/cryptography/rsa/PrivateKey.hh>
# include <elle/cryptography/rsa/defaults.hh>

ELLE_OPERATOR_RELATIONALS();

//
// ---------- Class -----------------------------------------------------------
//

namespace elle
{
  namespace cryptography
  {
    namespace rsa
    {
      /// Represent a cryptographic key pair _i.e_ a pair of public and
      /// private keys.
      ///
      /// Note that the public key is always written as a capital 'K'
      /// while a private key is noted with a lower-case 'k'.
      class KeyPair:
        public elle::Printable
      {
      public:
        /*-------------.
        | Construction |
        `-------------*/
      public:
        KeyPair(PublicKey const& K, PrivateKey const& k);
        KeyPair(std::shared_ptr<PublicKey> K,
                std::shared_ptr<PrivateKey> k);
        KeyPair(PublicKey&& K,
                PrivateKey&& k);
        KeyPair(KeyPair const& other);
        KeyPair(KeyPair&& other);
        virtual
        ~KeyPair();

        /*--------.
        | Methods |
        `--------*/
      public:
        /// Return the public key.
        PublicKey const&
        K() const;
        /// Return the private key.
        PrivateKey const&
        k() const;
        /// Return the key pair's size in bytes.
        uint32_t
        size() const;
        /// Return the key pair's length in bits.
        uint32_t
        length() const;

# if defined(ELLE_CRYPTOGRAPHY_ROTATION)
        /*---------.
        | Rotation |
        `---------*/
      public:
        explicit
        /// Deduce a keypair based out of the given seed.
        KeyPair(Seed const& seed);
# endif

        /*----------.
        | Operators |
        `----------*/
      public:
        bool
        operator ==(KeyPair const& other) const;
        ELLE_OPERATOR_NO_ASSIGNMENT(KeyPair);

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
        KeyPair(elle::serialization::SerializerIn& serializer);
        void
        serialize(elle::serialization::Serializer& serializer);
        typedef elle::serialization_tag serialization_tag;

        /*-----------.
        | Attributes |
        `-----------*/
      private:
        /// The public key.
        ELLE_ATTRIBUTE_R(std::shared_ptr<PublicKey>, public_key);
        /// The private key.
        ELLE_ATTRIBUTE_R(std::shared_ptr<PrivateKey>, private_key);
      };
    }
  }
}

//
// ---------- Generator -------------------------------------------------------
//

namespace elle
{
  namespace cryptography
  {
    namespace rsa
    {
      namespace keypair
      {
        /*----------.
        | Functions |
        `----------*/

        /// Return a pair composed of the public and private key of a
        /// freshly generated RSA key pair.
        ///
        /// Note that the length is in bits.
        KeyPair
        generate(uint32_t const length);

        namespace der
        {
          /*----------.
          | Functions |
          `----------*/

          /// Encode the key pair in DER.
          elle::Buffer
          encode(KeyPair const& keypair);
          /// Decode the key pair from a DER representation.
          KeyPair
          decode(elle::ConstWeakBuffer const& buffer);
        }
      }
    }
  }
}

# include <elle/cryptography/rsa/KeyPair.hxx>

#endif
