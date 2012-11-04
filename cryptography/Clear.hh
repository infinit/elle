#ifndef ELLE_CRYPTOGRAPHY_CLEAR_HH
# define ELLE_CRYPTOGRAPHY_CLEAR_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/Buffer.hh>

# include <elle/io/Dumpable.hh>

# include <elle/concept/Uniquable.hh>

# include <elle/serialize/HexadecimalArchive.hh>

# include <elle/cryptography/fwd.hh>

namespace elle
{
  namespace cryptography
  {
    /// XXX
    class Clear:
      public elle::io::Dumpable,
      public elle::concept::MakeUniquable<
        Clear,
        elle::serialize::HexadecimalArchive
      >
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      Clear() // XXX[to deserialize]
      {
      }
      explicit
      Clear(elle::Natural64 const size);
      Clear(Clear const& other);
      Clear(Clear&& other);

      /*----------.
      | Operators |
      `----------*/
    public:
      Boolean
      operator ==(Clear const& other) const;
      Boolean
      operator <(Clear const& other) const;
      Boolean
      operator <=(Clear const& other) const;
      ELLE_OPERATOR_NEQ(Clear);
      ELLE_OPERATOR_GT(Clear);
      ELLE_OPERATOR_GTE(Clear);
      ELLE_OPERATOR_NO_ASSIGNMENT(Clear);

      Boolean
      operator ==(Plain const& other) const;
      ELLE_OPERATOR_NEQ(Plain);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Clear);

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_RX(Buffer, buffer);
    };
  }
}

# include <elle/cryptography/Clear.hxx>

#endif
