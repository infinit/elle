#ifndef NUCLEUS_PROTON_CLEF_HH
# define NUCLEUS_PROTON_CLEF_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/Printable.hh>
# include <elle/serialize/construct.hh>

# include <nucleus/proton/Address.hh>

# include <cryptography/SecretKey.hh>

namespace nucleus
{
  namespace proton
  {
    /// Identify a permanent block through its address in the storage layer
    /// and the secret key required for decrypting the block content.
    class Clef:
      public elle::Printable
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      Clef(Address const& address,
           cryptography::SecretKey const& secret);
      Clef(Clef const& other);
      ELLE_SERIALIZE_CONSTRUCT_DECLARE(Clef);

      /*----------.
      | Operators |
      `----------*/
    public:
      ELLE_OPERATOR_NO_ASSIGNMENT(Clef);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Clef);

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_R(Address, address);
      ELLE_ATTRIBUTE_R(cryptography::SecretKey, secret);
    };
  }
}

# include <nucleus/proton/Clef.hxx>

#endif
