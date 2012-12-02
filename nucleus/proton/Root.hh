#ifndef NUCLEUS_PROTON_ROOT_HH
# define NUCLEUS_PROTON_ROOT_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>

# include <nucleus/proton/Height.hh>
# include <nucleus/proton/Capacity.hh>
# include <nucleus/proton/Handle.hh>

namespace nucleus
{
  namespace proton
  {
    /// Represent a tree through its most fundamental elements
    /// being its height and the handle to its root block.
    class Root:
      public elle::Printable
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      Root(); // XXX[deserialize]

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // serialize
      ELLE_SERIALIZE_FRIEND_FOR(Root);

      /*----------.
      | Operators |
      `----------*/
    public:
      ELLE_OPERATOR_NO_ASSIGNMENT(Root);

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_R(Height, height);
      ELLE_ATTRIBUTE_R(Capacity, capacity);
      ELLE_ATTRIBUTE_R(Handle, block);
    };
  }
}

# include <nucleus/proton/Root.hxx>

#endif
