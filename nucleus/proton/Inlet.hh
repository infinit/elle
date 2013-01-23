#ifndef NUCLEUS_PROTON_INLET_HH
# define NUCLEUS_PROTON_INLET_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>

# include <nucleus/proton/Footprint.hh>
# include <nucleus/proton/Handle.hh>
# include <nucleus/proton/State.hh>

# include <boost/noncopyable.hpp>

namespace nucleus
{
  namespace proton
  {
    /// Represent a nodule item i.e a descriptor of the child element
    /// being referenced be it either a sub-nodule block or a value block.
    ///
    /// An inlet embeds the item's key along with the address of the child
    /// block i.e through a handle.
    template <typename K>
    class Inlet:
      public elle::Printable,
      public elle::io::Dumpable,
      private boost::noncopyable
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      Inlet();
      Inlet(K const& k,
            Handle const& v);

      /*----------.
      | Operators |
      `----------*/
    public:
      ELLE_OPERATOR_NO_ASSIGNMENT(Inlet);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Inlet);

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_RW(K, key);
      ELLE_ATTRIBUTE_RX(Handle, value);
      ELLE_ATTRIBUTE_RW(Capacity, capacity);
      ELLE_ATTRIBUTE_RW(State, state);
      ELLE_ATTRIBUTE_RW(Footprint, footprint);
    };
  }
}

# include <nucleus/proton/Inlet.hxx>

#endif
