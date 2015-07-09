#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
# ifndef INFINIT_CRYPTOGRAPHY_LEGACY_INPUT_HH
#  define INFINIT_CRYPTOGRAPHY_LEGACY_INPUT_HH

#  include <cryptography/fwd.hh>

#  include <elle/types.hh>
#  include <elle/attribute.hh>
#  include <elle/operator.hh>
#  include <elle/Buffer.hh>
#  include <elle/Printable.hh>

namespace infinit
{
  namespace cryptography
  {
    /// Represent an input for an cryptographic operation. This class
    /// can be constructed from a weak buffer which is basically a wrapper
    /// around a memory address and size.
    class Input:
      public elle::Printable
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      Input(elle::ConstWeakBuffer const& buffer); /* implicit */
      virtual
      ~Input() = default;
    private:
      Input(Input const&);
      Input(Input&);

      /*----------.
      | Operators |
      `----------*/
    public:
      elle::Boolean
      operator ==(Input const& other) const;
      elle::Boolean
      operator <(Input const& other) const;
      elle::Boolean
      operator <=(Input const& other) const;
      ELLE_OPERATOR_NEQ(Input);
      ELLE_OPERATOR_GT(Input);
      ELLE_OPERATOR_GTE(Input);
      void
      operator =(Input const& other) = delete;
      Input&
      operator =(Input&& other) = default;

      elle::Boolean
      operator ==(Output const& other) const;
      elle::Boolean
      operator <(Output const& other) const;
      elle::Boolean
      operator <=(Output const& other) const;
      ELLE_OPERATOR_NEQ(Output);
      ELLE_OPERATOR_GT(Output);
      ELLE_OPERATOR_GTE(Output);

      /*----------.
      | Printable |
      `----------*/
    public:
      void
      print(std::ostream& stream) const override;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_R(elle::ConstWeakBuffer, buffer);
    };
  }
}

# endif
#else
# warning "LEGACY: this file should not have been included"
#endif
