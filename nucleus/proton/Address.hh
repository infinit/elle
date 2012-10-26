#ifndef NUCLEUS_PROTON_ADDRESS_HH
# define NUCLEUS_PROTON_ADDRESS_HH

# include <elle/cryptography/Digest.hh>
# include <elle/concept/Uniquable.hh>
# include <elle/serialize/construct.hh>
# include <elle/Printable.hh>

# include <nucleus/proton/Family.hh>
# include <nucleus/proton/Network.hh>

# include <nucleus/neutron/Component.hh>

# include <elle/operator.hh>
# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

    /// This class is one of the most fundamental of the nucleus library
    /// as it is used to identify a block in the storage layer.
    ///
    /// Note that this class includes a digest which is used so as to
    /// map an address onto a network's node. Indeed, every node is identified
    /// by a label which happens to be a digest as well. This way, both the
    /// node identifiers (label) and the block identifiers (address) lay
    /// in the same space.
    ///
    /// However the address class includes additional information such
    /// as the network, family---i.e the type of physical block---and the
    /// component---i.e the type of logical block.
    ///
    /// Noteworthy is that the network, family and component are also included
    /// in the generation of the address' digest.
    class Address:
      public elle::Printable,
      public elle::concept::MakeUniquable<Address>
    {
      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Return an unused address.
      static
      Address const&
      null();
      /// Return a not-yet-used address i.e an address with a irrelevant
      /// value (for now) but whose state, once serialized, should reflect
      /// the one of a valid address; the size of the address once serialized
      /// should be extremely close (or equal) to the one of a valid address.
      static
      Address const&
      some();

      //
      // enumerations
      //
    public:
      enum class Type
      {
        null,
        some,
        valid
      };

      //
      // construction
      //
    public:
      Address(); // XXX[to deserialize]
      ELLE_SERIALIZE_CONSTRUCT(Address) {}
      template <typename... T>
      Address(Network const& network,
              const Family&,
              const neutron::Component&,
              const T&...);
      Address(Address const& other);
    private:
      Address(Type const type);

      //
      // methods
      //
    public:
      /// this method returns a unique representation of the address.
      elle::String const
      unique() const;

      //
      // operators
      //
    public:
      elle::Boolean
      operator ==(Address const& other) const;
      elle::Boolean
      operator <(Address const& other) const;
      elle::Boolean
      operator <=(Address const& other) const;
      ELLE_OPERATOR_NEQ(Address);
      ELLE_OPERATOR_GT(Address);
      ELLE_OPERATOR_GTE(Address);
      ELLE_OPERATOR_ASSIGNMENT(Address);

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Address);
      // printable
      virtual
      void
      print(std::ostream& stream) const;

      //
      // attributes
      //
    private:
      ELLE_ATTRIBUTE(Type, type);
      ELLE_ATTRIBUTE_R(Network, network);
      ELLE_ATTRIBUTE_R(Family, family);
      ELLE_ATTRIBUTE_R(neutron::Component, component);
      ELLE_ATTRIBUTE(elle::cryptography::Digest, digest);
    };

  }
}

#include <nucleus/proton/Address.hxx>

#endif
