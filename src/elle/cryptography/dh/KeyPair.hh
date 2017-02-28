#ifndef ELLE_CRYPTOGRAPHY_DH_KEYPAIR_HH
# define ELLE_CRYPTOGRAPHY_DH_KEYPAIR_HH

//
// ---------- Class -----------------------------------------------------------
//

# include <iosfwd>
# include <utility>

# include <boost/operators.hpp>

# include <elle/cryptography/fwd.hh>
# include <elle/cryptography/Oneway.hh>
# include <elle/cryptography/Cipher.hh>
# include <elle/cryptography/dh/PublicKey.hh>
# include <elle/cryptography/dh/PrivateKey.hh>


namespace elle
{
  namespace cryptography
  {
    namespace dh
    {
      /// Represent a cryptographic key pair _i.e_ a pair of public and
      /// private keys.
      ///
      /// Note that the public key is always written as a capital 'K'
      /// while a private key is noted with a lower-case 'k'.
      class KeyPair
        : public elle::Printable
        , private boost::totally_ordered<KeyPair>
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
    namespace dh
    {
      namespace keypair
      {
        /*----------.
        | Functions |
        `----------*/

        /// Return a pair composed of the public and private key of a
        /// freshly generated DH key pair.
        KeyPair
        generate();
      }
    }
  }
}

# include <elle/cryptography/dh/KeyPair.hxx>

#endif
