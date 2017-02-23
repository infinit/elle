#ifndef ELLE_CRYPTOGRAPHY_DSA_KEYPAIR_HH
# define ELLE_CRYPTOGRAPHY_DSA_KEYPAIR_HH

# include <iosfwd>
# include <utility>

# include <elle/types.hh>
# include <elle/serialization/Serializer.hh>
# include <elle/serialization.hh>

# include <elle/cryptography/fwd.hh>
# include <elle/cryptography/Oneway.hh>
# include <elle/cryptography/Cipher.hh>
# include <elle/cryptography/dsa/PublicKey.hh>
# include <elle/cryptography/dsa/PrivateKey.hh>
# include <elle/cryptography/dsa/defaults.hh>

ELLE_OPERATOR_RELATIONALS();

namespace elle
{
  namespace cryptography
  {
    namespace dsa
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
        KeyPair(PublicKey const& K,
                PrivateKey const& k);
        KeyPair(PublicKey&& K,
                PrivateKey&& k);
        explicit
        KeyPair(KeyPair const& other);
        KeyPair(KeyPair&& other);
        virtual
        ~KeyPair() = default;

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
        ELLE_ATTRIBUTE(std::unique_ptr<PublicKey>, K);
        /// The private key.
        ELLE_ATTRIBUTE(std::unique_ptr<PrivateKey>, k);
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
    namespace dsa
    {
      namespace keypair
      {
        /*----------.
        | Functions |
        `----------*/

        /// Return a pair composed of the public and private key of a
        /// freshly generated DSA key pair.
        ///
        /// Note that the length is in bits.
        KeyPair
        generate(uint32_t const length,
                 Oneway const digest_algorithm =
                   defaults::digest_algorithm);
      }
    }
  }
}

# include <elle/cryptography/dsa/KeyPair.hxx>

#endif
