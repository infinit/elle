#ifndef INFINIT_CRYPTOGRAPHY_KEYPAIR_HH
# define INFINIT_CRYPTOGRAPHY_KEYPAIR_HH

# include <cryptography/fwd.hh>
# include <cryptography/Cryptosystem.hh>

# include <elle/types.hh>
# include <elle/Printable.hh>
# include <elle/operator.hh>
# include <elle/concept/Uniquable.hh>
# include <elle/serialize/fwd.hh>
# include <elle/serialize/construct.hh>
# include <elle/serialize/DynamicFormat.hh>

# include <utility>
ELLE_OPERATOR_RELATIONALS();

# include <iosfwd>

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
      public elle::serialize::DynamicFormat<KeyPair>,
      public elle::concept::MakeUniquable<KeyPair>,
      public elle::Printable
    {
      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Return a brand new, freshly generated key pair of the
      /// given length.
      ///
      /// Note that the length is given in bits.
      static
      KeyPair
      generate(Cryptosystem const cryptosystem,
               elle::Natural32 const length);

      /*-------------.
      | Construction |
      `-------------*/
    public:
      KeyPair(); // XXX[to deserialize]
      KeyPair(PublicKey const& K,
              PrivateKey const& k);
      KeyPair(PublicKey&& K,
              PrivateKey&& k);
      explicit
# if defined(ELLE_CRYPTOGRAPHY_ROTATION)
      KeyPair(Seed const& seed);
# endif
      KeyPair(KeyPair const& other);
      KeyPair(KeyPair&& other);
      ELLE_SERIALIZE_CONSTRUCT_DECLARE(KeyPair);

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
      /// Return the cryptosystem provided by this key pair.
      Cryptosystem
      cryptosystem() const;
      /// Return the key pair's size in bytes.
      elle::Natural32
      size() const;
      /// Return the key pair's length in bits.
      elle::Natural32
      length() const;

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
      ELLE_ATTRIBUTE(std::unique_ptr<PublicKey>, K);
      /// The private key.
      ELLE_ATTRIBUTE(std::unique_ptr<PrivateKey>, k);
    };
  }
}

# include <cryptography/KeyPair.hxx>

#endif
