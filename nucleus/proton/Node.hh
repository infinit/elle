#ifndef NUCLEUS_PROTON_NODE_HH
# define NUCLEUS_PROTON_NODE_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/io/Dumpable.hh>
# include <elle/utility/Factory.hh>
# include <elle/serialize/construct.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/neutron/Component.hh>

namespace nucleus
{
  namespace proton
  {
    /// Provide an abstraction for all the blocks composing a porcupine
    /// tree being either internal nodules (seam and quill) or leave
    /// values (catalog, data and reference).
    class Node:
      public elle::serialize::Serializable<>,
      public elle::io::Dumpable,
      public elle::Printable
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      Node();
      Node(Node const&) = default;
      ELLE_SERIALIZE_CONSTRUCT_DECLARE(Node);

      //
      // methods
      //
    public:
      /// XXX
      void
      nest(Nest& nest);
      /// XXX
      Nest&
      nest();
      /// XXX
      Footprint
      footprint() const;
      /// XXX
      void
      footprint(Footprint const footprint);
      /// XXX
      State
      state() const;
      /// XXX
      void
      state(State const state);

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 margin) const;
      // printable
      virtual
      void
      print(std::ostream& stream) const;

      //
      // attributes
      //
    private:
      Nest* _nest;
      Footprint _footprint;
      State _state;
    };
  }
}

# include <nucleus/proton/Node.hxx>

#endif
