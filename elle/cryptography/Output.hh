#ifndef ELLE_CRYPTOGRAPHY_OUTPUT_HH
# define ELLE_CRYPTOGRAPHY_OUTPUT_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/Buffer.hh>
# include <elle/Printable.hh>

# include <elle/io/Dumpable.hh>

# include <elle/cryptography/fwd.hh>

# include <elle/concept/Uniquable.hh>
# include <elle/serialize/HexadecimalArchive.hh>

namespace elle
{
  namespace cryptography
  {
    /// XXX
    class Output:
      public elle::io::Dumpable,
      public elle::Printable,
      public elle::concept::MakeUniquable<
        Output,
        elle::serialize::HexadecimalArchive
      >
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      Output() // XXX[to deserialize]
      {
      }
      explicit
      Output(elle::Natural64 const size);
      Output(Output const& other);
      Output(Output&& other);

      /*----------.
      | Operators |
      `----------*/
    public:
      Boolean
      operator ==(Output const& other) const;
      Boolean
      operator <(Output const& other) const;
      Boolean
      operator <=(Output const& other) const;
      ELLE_OPERATOR_NEQ(Output);
      ELLE_OPERATOR_GT(Output);
      ELLE_OPERATOR_GTE(Output);
      ELLE_OPERATOR_ASSIGNMENT(Output); // XXX

      Boolean
      operator ==(Input const& other) const;
      Boolean
      operator <(Input const& other) const;
      Boolean
      operator <=(Input const& other) const;
      ELLE_OPERATOR_NEQ(Input);
      ELLE_OPERATOR_GT(Input);
      ELLE_OPERATOR_GTE(Input);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Output);
      // printable
      virtual
      void
      print(std::ostream& stream) const;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_RX(Buffer, buffer);
    };
  }
}

# include <elle/cryptography/Output.hxx>

#endif
