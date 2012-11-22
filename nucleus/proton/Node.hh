#ifndef NUCLEUS_PROTON_NODE_HH
# define NUCLEUS_PROTON_NODE_HH

# include <elle/types.hh>
# include <elle/serialize/Serializable.hh>
# include <elle/io/Dumpable.hh>
# include <elle/utility/Factory.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/neutron/Component.hh>

namespace nucleus
{
  namespace proton
  {

    /// XXX
    class Node:
      public elle::serialize::Serializable<>,
      public elle::io::Dumpable,
      public elle::Printable
    {
      //
      // enumerations
      //
    public:
      // XXX[peut etre a revoir: mettre dans Contents?]
      enum Type
        {
          TypeSeamData,
          TypeQuillData,
          TypeValueData,
          TypeSeamCatalog,
          TypeQuillCatalog,
          TypeValueCatalog,
          TypeSeamReference,
          TypeQuillReference,
          TypeValueReference
        };


      //
      // static methods
      //
    public:
      /// XXX
      static
      elle::Boolean
      setup();

      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Return the factory capable of building nucleus class instances.
      static
      elle::utility::Factory<Type> const&
      factory();

      //
      // constructors & destructors
      //
    public:
      /// XXX
      Node();

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
      Capacity
      capacity() const;
      /// XXX
      void
      capacity(Capacity const capacity);
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

      // serialize
      ELLE_SERIALIZE_FRIEND_FOR(Node);

      //
      // attributes
      //
    private:
      Nest* _nest;
      Footprint _footprint;
      Capacity _capacity;
      State _state;
    };

  }
}

# include <nucleus/proton/Node.hxx>

#endif
