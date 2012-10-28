#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Network.hh>
#include <nucleus/Exception.hh>

#include <elle/format/hexadecimal.hh>

namespace nucleus
{
  namespace proton
  {

    /*---------------.
    | Static Methods |
    `---------------*/

    Address const&
    Address::null()
    {
      static Address address(Address::Type::null);

      return (address);
    }

    Address const&
    Address::some()
    {
      static Address address(Address::Type::some);

      return (address);
    }

//
// ---------- constructors & destructors --------------------------------------
//

    Address::Address():
      _valid(nullptr)
    {
    }

    Address::Address(Address const& other):
      _type(other._type),
      _valid(nullptr)
    {
      if (other._valid != nullptr)
        {
          ELLE_ASSERT(other._valid->digest() != nullptr);

          this->_valid =
            new Address::Valid(
              other._valid->network(),
              other._valid->family(),
              other._valid->component(),
              new elle::cryptography::Digest(*other._valid->digest()));
        }
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

    Address::Valid::Valid():
      _digest(nullptr)
    {
    }

    Address::Valid::Valid(Network const& network,
                          Family const& family,
                          neutron::Component const& component,
                          elle::cryptography::Digest* digest):
      _network(network),
      _family(family),
      _component(component),
      _digest(digest)
    {
    }

    Address::Valid::~Valid()
    {
      delete this->_digest;
    }

//
// ---------- methosd ---------------------------------------------------------
//

    elle::String const
    Address::unique() const
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);
      ELLE_ASSERT(this->_valid->digest() != nullptr);

      // note that a unique element i.e the digest has already been computed
      // when the address was created.
      //
      // therefore, this method simply returns a string representation of
      // the digest.
      return (elle::format::hexadecimal::encode(
                reinterpret_cast<const char*>(
                  this->_valid->digest()->region.contents),
                this->_valid->digest()->region.size));
    }

    Network const&
    Address::network() const
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      return (this->_valid->network());
    }

    Family const&
    Address::family() const
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      return (this->_valid->family());
    }

    neutron::Component const&
    Address::component() const
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      return (this->_valid->component());
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
        {
          ELLE_ASSERT(this->_valid != nullptr);
          ELLE_ASSERT(this->_valid->digest() != nullptr);
          ELLE_ASSERT(other._valid != nullptr);
          ELLE_ASSERT(other._valid->digest() != nullptr);

          return (*this->_valid->digest() == *other._valid->digest());
        }
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
        {
          ELLE_ASSERT(this->_valid != nullptr);
          ELLE_ASSERT(this->_valid->digest() != nullptr);
          ELLE_ASSERT(other._valid != nullptr);
          ELLE_ASSERT(other._valid->digest() != nullptr);

          return (*this->_valid->digest() < *other._valid->digest());
        }
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
        {
          ELLE_ASSERT(this->_valid != nullptr);
          ELLE_ASSERT(this->_valid->digest() != nullptr);
          ELLE_ASSERT(other._valid != nullptr);
          ELLE_ASSERT(other._valid->digest() != nullptr);

          return (*this->_valid->digest() <= *other._valid->digest());
        }
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
      switch (this->_type)
        {
        case Address::Type::null:
          {
            std::cout << alignment << "[Address] " << elle::none << std::endl;

            break;
          }
        case Address::Type::some:
          {
            std::cout << alignment << "[Address] " << "(undef)" << std::endl;

            break;
          }
        case Address::Type::valid:
          {
            ELLE_ASSERT(this->_valid != nullptr);
            ELLE_ASSERT(this->_valid->digest() != nullptr);

            // display the name.
            std::cout << alignment << "[Address] " << this << std::endl;

            if (this->_valid->network().Dump(margin + 2) == elle::Status::Error)
              escape("XXX");

            // display the family.
            std::cout << alignment << elle::io::Dumpable::Shift << "[Family] "
                      << this->_valid->family() << std::endl;

            // display the component.
            std::cout << alignment << elle::io::Dumpable::Shift
                      << "[Component] "
                      << this->_valid->component() << std::endl;

            // dump the digest.
            if (this->_valid->digest()->Dump(margin + 2) == elle::Status::Error)
              escape("unable to dump the digest");

            break;
          }
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
            ELLE_ASSERT(this->_valid != nullptr);

            stream << "address{"
                   << this->_valid->network()
                   << ", "
                   << this->_valid->family()
                   << ", "
                   << this->_valid->component()
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
