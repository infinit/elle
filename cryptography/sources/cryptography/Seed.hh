#ifndef INFINIT_CRYPTOGRAPHY_SEED_HH
# define INFINIT_CRYPTOGRAPHY_SEED_HH

# include <cryptography/fwd.hh>

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/Buffer.hh>
# include <elle/Printable.hh>
# include <elle/serialize/fwd.hh>

namespace infinit
{
  namespace cryptography
  {
    /// XXX
    class Seed:
      public elle::Printable
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      Seed(KeyPair const& pair); // XXX to deduce seed length
      // XXX load constructor

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Rotate the current seed with the given private key, leading to a
      /// new seed.
      Seed
      rotate(PrivateKey const& k) const;
      /// Return the base seed which has been rotated with the complementary
      /// private key to the given public key.
      Seed
      derive(PublicKey const& K) const;

      /*----------.
      | Operators |
      `----------*/
    public:
      elle::Boolean
      operator ==(Seed const& other) const;
      ELLE_OPERATOR_NEQ(Seed);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Seed);

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE(elle::Buffer, buffer);
    };
  }
}

# include <cryptography/Seed.hxx>

#endif
