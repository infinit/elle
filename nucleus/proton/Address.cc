#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Network.hh>
#include <nucleus/Exception.hh>

#include <elle/format/hexadecimal.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- definitions -----------------------------------------------------
//

    Address const Address::null(Address::Type::null);
    Address const Address::some(Address::Type::some);

//
// ---------- constructors & destructors --------------------------------------
//

    Address::Address()
    {
    }

    Address::Address(Address const& other):
      _type(other._type),
      _network(other._network),
      _family(other._family),
      _component(other._component),
      _digest(other._digest)
    {
    }

    Address::Address(Type const type):
      _type(type)
    {
      switch (type)
        {
        case Type::valid:
          {
            throw Exception("valid addresses cannot be built through this "
                            "constructor");
          }
        case Type::null:
        case Type::some:
          {
            // Nothing to do; this is the right way to construct such special
            // addresses
          }
        }
    }

//
// ---------- methosd ---------------------------------------------------------
//

    elle::String const
    Address::unique() const
    {
      assert(this->_type == Type::valid);

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
      if (this == &other)
        return (true);

      if (this->_type != other._type)
        return (false);

      if (this->_type == Type::valid)
        return (this->_digest == other._digest);
      else
        return (true);
    }

    elle::Boolean
    Address::operator <(Address const& other) const
    {
      if (this == &other)
        return (false);

      if (this->_type != other._type)
        return (this->_type < other._type);

      if (this->_type == Type::valid)
        return (this->_digest < other._digest);
      else
        return (false);
    }

    elle::Boolean
    Address::operator <=(Address const& other) const
    {
      if (this == &other)
        return (true);

      if (this->_type != other._type)
        return (this->_type <= other._type);

      if (this->_type == Type::valid)
        return (this->_digest <= other._digest);
      else
        return (false);
    }

//
// ---------- dumpable --------------------------------------------------------
//

    elle::Status
    Address::Dump(elle::Natural32           margin) const
    {
      elle::String      alignment(margin, ' ');

      // check the value.
      if (*this == Address::null)
        {
          std::cout << alignment << "[Address] " << elle::none << std::endl;
        }
      else if (*this == Address::some)
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
      switch (this->_type)
        {
        case Type::valid:
          {
            stream << "address{"
                   << this->_network
                   << ", "
                   << this->_family
                   << ", "
                   << this->_component
                   << ", "
                   << this->unique()
                   << "}";
            break;
          }
        case Type::null:
          {
            stream << "address(null)";
            break;
          }
        case Type::some:
          {
            stream << "address(some)";
            break;
          }
        }
    }

  }
}
