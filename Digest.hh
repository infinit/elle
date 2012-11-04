#ifndef ELLE_CRYPTOGRAPHY_DIGEST_HH
# define ELLE_CRYPTOGRAPHY_DIGEST_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/Buffer.hh>

# include <elle/io/Dumpable.hh>

# include <elle/concept/Uniquable.hh>
# include <elle/serialize/HexadecimalArchive.hh>

namespace elle
{
  namespace cryptography
  {
    /// XXX
    class Digest:
      public elle::io::Dumpable,
      public elle::concept::MakeUniquable<
        Digest,
        elle::serialize::HexadecimalArchive
      >
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      Digest() // XXX[to deserialize]
      {
      }
      explicit
      Digest(elle::Natural64 const size);
      Digest(Digest const& other);
      Digest(Digest&& other);

      /*----------.
      | Operators |
      `----------*/
    public:
      Boolean
      operator ==(Digest const& other) const;
      Boolean
      operator <(Digest const& other) const;
      Boolean
      operator <=(Digest const& other) const;
      ELLE_OPERATOR_NEQ(Digest);
      ELLE_OPERATOR_GT(Digest);
      ELLE_OPERATOR_GTE(Digest);
      ELLE_OPERATOR_NO_ASSIGNMENT(Digest);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Digest);

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_RX(Buffer, buffer);
    };
  }
}

# include <elle/cryptography/Digest.hxx>

#endif
