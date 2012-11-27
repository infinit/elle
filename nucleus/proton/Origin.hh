#ifndef NUCLEUS_PROTON_ORIGIN_HH
# define NUCLEUS_PROTON_ORIGIN_HH

# include <elle/types.hh>
# include <elle/attributes.hh>
# include <elle/operator.hh>

namespace nucleus
{
  namespace proton
  {
    /// XXX
    class Origin:
      public elle::Printable,
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      Origin();

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // serialize
      ELLE_SERIALIZE_FRIEND_FOR(Origin);

      /*-----------.
      | Attributes |
      `-----------*/

      Mode _mode;
      Height _height;
      Capacity _capacity;
      ELLE_ATTRIBUTE_X(Handle, root); // XXX[remove X}]

    };
  }
}

#endif
