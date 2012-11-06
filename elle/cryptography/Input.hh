#ifndef ELLE_CRYPTOGRAPHY_INPUT_HH
# define ELLE_CRYPTOGRAPHY_INPUT_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/Buffer.hh>

# include <elle/io/Dumpable.hh>

# include <elle/cryptography/fwd.hh>

# include <elle/serialize/fwd.hh>

namespace elle
{
  namespace cryptography
  {
    /// XXX
    class Input:
      public elle::io::Dumpable
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      Input(WeakBuffer const& buffer);

      /*----------.
      | Operators |
      `----------*/
    public:
      Boolean
      operator ==(Input const& other) const;
      Boolean
      operator <(Input const& other) const;
      Boolean
      operator <=(Input const& other) const;
      ELLE_OPERATOR_NEQ(Input);
      ELLE_OPERATOR_GT(Input);
      ELLE_OPERATOR_GTE(Input);
      ELLE_OPERATOR_ASSIGNMENT(Input); // XXX

      Boolean
      operator ==(Output const& other) const;
      Boolean
      operator <(Output const& other) const;
      Boolean
      operator <=(Output const& other) const;
      ELLE_OPERATOR_NEQ(Output);
      ELLE_OPERATOR_GT(Output);
      ELLE_OPERATOR_GTE(Output);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Input);
      // printable
      virtual
      void
      print(std::ostream& stream) const;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_R(WeakBuffer, buffer);
    };
  }
}

# include <elle/cryptography/Input.hxx>

#endif
