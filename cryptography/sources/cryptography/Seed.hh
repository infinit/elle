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
    // XXX[no copy?]
    {
      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Return a randomly generated seed suitable for the given key pair.
      static
      Seed
      generate(KeyPair const& pair);

      /*-------------.
      | Construction |
      `-------------*/
    public:
      explicit
      Seed(elle::Buffer&& buffer);
      Seed(Seed&& other);
      ELLE_SERIALIZE_CONSTRUCT_DECLARE(Seed);

      /*----------.
      | Operators |
      `----------*/
    public:
      elle::Boolean
      operator ==(Seed const& other) const;
      ELLE_OPERATOR_NEQ(Seed);
      // XXX NO ASSIGNMENT

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
      ELLE_ATTRIBUTE_R(elle::Buffer, buffer);
    };
  }
}

# include <cryptography/Seed.hxx>

#endif
