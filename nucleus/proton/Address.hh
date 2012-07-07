#ifndef NUCLEUS_PROTON_ADDRESS_HH
# define NUCLEUS_PROTON_ADDRESS_HH

# include <elle/cryptography/fwd.hh>
# include <elle/concept/Serializable.hh>
# include <elle/concept/Fileable.hh>
# include <elle/concept/Uniquable.hh>

# include <nucleus/proton/Family.hh>

# include <nucleus/neutron/Component.hh>

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
    class Address
      : public elle::radix::Object
      , public elle::concept::MakeFileable<Address>
      , public elle::concept::MakeUniquable<Address>
    {
    public:
      //
      // constants
      //
      static const Address      Null;
      static Address            Any;
      static Address&           Some;

      //
      // static methods
      //
      static elle::Status       Initialize();
      static elle::Status       Clean();

      //
      // constructors & destructors
      //
      Address();
      Address(const Address&);
      ~Address();

      //
      // methods
      //
      template <typename T,
                typename... TT>
      elle::Status              Create(const Family&,
                                       const neutron::Component&,
                                       const T&,
                                       const TT&...);

      /// this method returns a unique representation of the address.
      elle::String
      unique() const;

      //
      // interfaces
      //

      // object
      declare(Address);
      elle::Boolean             operator==(const Address&) const;
      elle::Boolean             operator<(const Address&) const;

      // dumpable
      elle::Status              Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Family                    family;
      neutron::Component        component;

      elle::cryptography::Digest* digest;
    };

  }
}

#include <nucleus/proton/Address.hxx>

#endif
