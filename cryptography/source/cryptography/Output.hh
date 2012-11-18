#ifndef INFINIT_CRYPTOGRAPHY_OUTPUT_HH
# define INFINIT_CRYPTOGRAPHY_OUTPUT_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/Buffer.hh>
# include <elle/Printable.hh>

# include <elle/io/Dumpable.hh>

# include <cryptography/fwd.hh>

namespace infinit
{
  namespace cryptography
  {
    /// Represent an output of a cryptographic operation. Note that one
    /// can retrieve the buffer embedded in this class so as to use it
    /// outside of the cryptography library.
    class Output:
      public elle::io::Dumpable,
      public elle::Printable
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
      elle::Boolean
      operator ==(Output const& other) const;
      elle::Boolean
      operator <(Output const& other) const;
      elle::Boolean
      operator <=(Output const& other) const;
      ELLE_OPERATOR_NEQ(Output);
      ELLE_OPERATOR_GT(Output);
      ELLE_OPERATOR_GTE(Output);
      ELLE_OPERATOR_ASSIGNMENT(Output); // XXX

      elle::Boolean
      operator ==(Input const& other) const;
      elle::Boolean
      operator <(Input const& other) const;
      elle::Boolean
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
      ELLE_ATTRIBUTE_RX(elle::Buffer, buffer);
    };
  }
}

# include <cryptography/Output.hxx>

#endif
