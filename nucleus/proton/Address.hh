#ifndef NUCLEUS_PROTON_ADDRESS_HH
# define NUCLEUS_PROTON_ADDRESS_HH

# include <elle/Printable.hh>
# include <elle/concept/Uniquable.hh>
# include <elle/serialize/construct.hh>

# include <cryptography/Digest.hh>
# include <cryptography/oneway.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

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
      /*----------.
      | Constants |
      `----------*/
    public:
      struct Constants
      {
        static cryptography::oneway::Algorithm const oneway_algorithm;
      };

      /*---------------.
      | Static Methods |
      `---------------*/
    public:
      /// Return an unused address.
      static
      Address const&
      null();

      /*-------------.
      | Enumerations |
      `-------------*/
    public:
      enum class Type
      {
        null,
        valid
      };

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Address(); // XXX[to deserialize]
      ELLE_SERIALIZE_CONSTRUCT(Address)
      {
        _valid = nullptr;
      }
      template <typename... T>
      Address(Network const& network,
              const Family&,
              const neutron::Component&,
              const T&...);
      Address(Address const& other);
      ~Address();
    private:
      Address(Type const type);

      /*--------.
      | Methods |
      `--------*/
    public:
      /// Return a unique representation of the address.
      elle::String const
      unique() const;
      /// Return the address' network.
      Network const&
      network() const;
      /// Return the address' family.
      Family const&
      family() const;
      /// Return the address' component.
      neutron::Component const&
      component() const;

      /*----------.
      | Operators |
      `----------*/
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
      ELLE_OPERATOR_ASSIGNMENT(Address); // XXX

      /*-----------.
      | Interfaces |
      `-----------*/
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

    public:
      /*-----------.
      | Structures |
      `-----------*/
      struct Valid
      {
        // construction
      public:
        Valid();
        Valid(Network const& network,
              Family const& family,
              neutron::Component const& component,
              cryptography::Digest const& digest);

      public:
        // serializable
        ELLE_SERIALIZE_FRIEND_FOR(Address::Valid);

        // attributes
      private:
        ELLE_ATTRIBUTE_R(Network, network);
        ELLE_ATTRIBUTE_R(Family, family);
        ELLE_ATTRIBUTE_R(neutron::Component, component);
        ELLE_ATTRIBUTE_R(cryptography::Digest, digest);
      };

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_R(Type, type);
      ELLE_ATTRIBUTE(Valid*, valid);
    };

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Address::Type const type);
  }
}

#include <nucleus/proton/Address.hxx>

#endif
