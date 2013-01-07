#ifndef INFINIT_CRYPTOGRAPHY_KEYPAIR_HH
# define INFINIT_CRYPTOGRAPHY_KEYPAIR_HH

# include <cryptography/PublicKey.hh>
# include <cryptography/PrivateKey.hh>

# include <elle/types.hh>
# include <elle/Printable.hh>
# include <elle/operator.hh>
# include <elle/concept/Uniquable.hh>
# include <elle/serialize/fwd.hh>
# include <elle/serialize/construct.hh>

# include <utility>
ELLE_OPERATOR_RELATIONALS();

namespace infinit
{
  namespace cryptography
  {
    /// Represent a cryptographic key pair _i.e_ a pair of public and
    /// private keys.
    ///
    /// Note that the public key is always written as a capital 'K'
    /// while a private key is noted with a lower-case 'k'.
    class KeyPair:
      public elle::concept::MakeUniquable<KeyPair>,
      public elle::Printable
    {
      /*------------------.
      | Static Attributes |
      `------------------*/
    public:
      struct Contexts
      {
        static ::EVP_PKEY_CTX* generate;
      };

      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Initialize the keypair contexts.
      static
      void
      initialize();
      /// Clean the keypair contexts.
      static
      void
      clean();
      /// Return a brand new, freshly generated key pair of the
      /// given length.
      static
      KeyPair
      generate(elle::Natural32 const length);

      /*-------------.
      | Construction |
      `-------------*/
    public:
      KeyPair(); // XXX[to deserialize]
      KeyPair(PublicKey const& K,
              PrivateKey const& k);
      KeyPair(::EVP_PKEY const* key);
      KeyPair(KeyPair const& pair);
      ELLE_SERIALIZE_CONSTRUCT_DECLARE(KeyPair);

      /*----------.
      | Operators |
      `----------*/
    public:
      elle::Boolean
      operator ==(KeyPair const& other) const;
      ELLE_OPERATOR_NO_ASSIGNMENT(KeyPair);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(KeyPair);

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      /// The public key.
      ELLE_ATTRIBUTE_R(PublicKey, K);
      /// The private key.
      ELLE_ATTRIBUTE_R(PrivateKey, k);
    };
  }
}

# include <cryptography/KeyPair.hxx>

#endif
