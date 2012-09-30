#ifndef NUCLEUS_PROTON_NETWORK_HH
# define NUCLEUS_PROTON_NETWORK_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/Printable.hh>
# include <elle/serialize/fwd.hh>

# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// this class identifies a network through a unique name.
    ///
    class Network:
      public elle::Printable
    {
      //
      // constants
      //
    public:
      static const Network      Null;

      //
      // constructors & destructors
      //
    public:
      Network(); // XXX[to deserialization]
      Network(elle::String const& name);

      //
      // operators
      //
    public:
      elle::Boolean
      operator ==(Network const& other) const;
      ELLE_OPERATOR_NEQ(Network);
      ELLE_OPERATOR_ASSIGNMENT(Network);

      //
      // interfaces
      //
    public:
      // serialize
      ELLE_SERIALIZE_FRIEND_FOR(Network);
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
    public:
      ELLE_ATTRIBUTE_R(elle::String, name);
    };

  }
}

#include <nucleus/proton/Network.hxx>

#endif
