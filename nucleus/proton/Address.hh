#ifndef NUCLEUS_PROTON_ADDRESS_HH
# define NUCLEUS_PROTON_ADDRESS_HH

# include <elle/cryptography/fwd.hh>
# include <elle/concept/Uniquable.hh>
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

    ///
    /// this class is one of the most fundamental of the nucleus library
    /// as it is used to identify a block in the storage layer.
    ///
    /// note that this class includes a digest which is used so as to
    /// map an address onto a network's node. indeed, every node is identified
    /// by a label which happen to be a digest as well.
    ///
    /// however the address class includes additional information such
    /// as the family---i.e the type of physical block---and the
    /// component---i.e the type of logical block.
    ///
    /// noteworthy is that the family and component are also included in
    /// the generation of the address' digest.
    ///
    class Address:
      public elle::Printable,
      public elle::concept::MakeUniquable<Address>
    {
      //
      // constants
      //
    public:
      static const Address      Null;
      static Address            Any;
      static Address&           Some;

      //
      // static methods
      //
    public:
      static elle::Status       Initialize();
      static elle::Status       Clean();

      //
      // construction
      //
    public:
      Address(); // XXX[to deserialize]
      Address(Address const& other);
      ~Address();

      //
      // methods
      //
    public:
      template <typename... T>
      elle::Status              Create(Network const& network,
                                       const Family&,
                                       const neutron::Component&,
                                       const T&...);

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
      elle::Status              Dump(const elle::Natural32 = 0) const;

      // serializable
      ELLE_SERIALIZE_FRIEND_FOR(Address);

      // printable
      virtual
      void
      print(std::ostream& stream) const;

      //
      // attributes
      //
      // XXX private:
    public:
      ELLE_ATTRIBUTE_R(Network, network);
      ELLE_ATTRIBUTE_R(Family, family);
      ELLE_ATTRIBUTE_R(neutron::Component, component);

      elle::cryptography::Digest* _digest;
    };

  }
}

#include <nucleus/proton/Address.hxx>

#endif
