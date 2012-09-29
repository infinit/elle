#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Network.hh>

#include <elle/format/hexadecimal.hh>

ELLE_LOG_COMPONENT("infinit.nucleus.proton.Address");

namespace nucleus
{
  namespace proton
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable can easily be used for comparing with invalid addresses.
    ///
    const Address               Address::Null;

    ///
    /// this variable is similar to Null except that it is constructed
    /// so as not to be considered "empty". this constant is particularly
    /// useful when one wants to know the footprint of such an Address type.
    ///
    Address                     Address::Any;

    ///
    /// this variable is an alias of Any.
    ///
    Address&                    Address::Some = Address::Any;

//
// ---------- constructors & destructors --------------------------------------
//

    Address::Address():
      _family(FamilyUnknown),
      _component(neutron::ComponentUnknown)
    {
      // XXX[to remove later since this contructor should not exist]
      // XXX[besides, this does not work because the address is constructed
      //     statically. thus, OneWay::Hash is called while nothing has been
      //     initialized nor the cryptography module, nor the log mechanism
      //     etc.]
      /* XXX
      if (elle::cryptography::OneWay::Hash(
            elle::serialize::make_tuple(this->_network,
                                        this->_family,
                                        this->_component),
            this->_digest) == elle::Status::Error)
        throw Exception("unable to hash the given parameter(s)");
      */
    }

    Address::Address(Address const& other):
      _network(other._network),
      _family(other._family),
      _component(other._component),
      _digest(other._digest)
    {
    }

//
// ---------- methosd ---------------------------------------------------------
//

    elle::String const
    Address::unique() const
    {
      // note that a unique element i.e the digest has already been computed
      // when the address was created.
      //
      // therefore, this method simply returns a string representation of
      // the digest.
      return (elle::format::hexadecimal::encode(
                reinterpret_cast<const char*>(this->_digest.region.contents),
                this->_digest.region.size));
    }

//
// ---------- operators -------------------------------------------------------
//

    elle::Boolean
    Address::operator ==(Address const& other) const
    {
      // check the address as this may actually be the same object.
      if (this == &other)
        return true;

      return (this->_digest == other._digest);
    }

    elle::Boolean
    Address::operator <(Address const& other) const
    {
      // check the address as this may actually be the same object.
      if (this == &other)
        return false;

      return (this->_digest < other._digest);
    }

    elle::Boolean
    Address::operator <=(Address const& other) const
    {
      // check the address as this may actually be the same object.
      if (this == &other)
        return true;

      return (this->_digest <= other._digest);
    }

//
// ---------- dumpable --------------------------------------------------------
//

    elle::Status
    Address::Dump(elle::Natural32           margin) const
    {
      elle::String      alignment(margin, ' ');

      // check the value.
      if (*this == Address::Null)
        {
          std::cout << alignment << "[Address] " << elle::none << std::endl;
        }
      else if (*this == Address::Some)
        {
          std::cout << alignment << "[Address] " << "(undef)" << std::endl;
        }
      else
        {
          // display the name.
          std::cout << alignment << "[Address] " << this << std::endl;

          if (this->_network.Dump(margin + 2) == elle::Status::Error)
            escape("XXX");

          // display the family.
          std::cout << alignment << elle::io::Dumpable::Shift << "[Family] "
                    << this->_family << std::endl;

          // display the component.
          std::cout << alignment << elle::io::Dumpable::Shift << "[Component] "
                    << this->_component << std::endl;

          // dump the digest.
          if (this->_digest.Dump(margin + 2) == elle::Status::Error)
            escape("unable to dump the digest");
        }

      return elle::Status::Ok;
    }

//
// ---------- printable -------------------------------------------------------
//

    void
    Address::print(std::ostream& stream) const
    {
      stream << "address("
             << this->_network
             << ", "
             << this->_family
             << ", "
             << this->_component
             << ", "
             << this->unique()
             << ")";
    }

  }
}
