#include <nucleus/neutron/Trait.hh>
#include <nucleus/Exception.hh>

#include <elle/io/Dumpable.hh>

namespace nucleus
{
  namespace neutron
  {

    /*---------------.
    | Static Methods |
    `---------------*/

    Trait const&
    Trait::null()
    {
      static Trait trait(Trait::Type::null);

      return (trait);
    }

    /*-------------.
    | Construction |
    `-------------*/

    Trait::Trait():
      _valid(nullptr)
    {
    }

    Trait::Trait(elle::String const& name,
                 elle::String const& value):
      _type(Type::valid),
      _valid(new Valid(name, value))
    {
    }

    Trait::Trait(Trait const& other):
      _type(other._type),
      _valid(nullptr)
    {
      if (other._valid != nullptr)
        this->_valid = new Valid(other._valid->name(),
                                 other._valid->value());
    }

    Trait::Trait(Type const type):
      _type(type),
      _valid(nullptr)
    {
      switch (this->_type)
        {
        case Type::null:
          {
            // Nothing to do; this is the right way to construct such special
            // traits.
            break;
          }
        case Type::valid:
          {
            throw Exception("valid traits cannot be built through this "
                            "constructor");
          }
        default:
          throw Exception("unknown trait type '%s'", this->_type);
        }
    }

    Trait::~Trait()
    {
      delete this->_valid;
    }

    Trait::Valid::Valid()
    {
    }

    Trait::Valid::Valid(elle::String const& name,
                        elle::String const& value):
      _name(name),
      _value(value)
    {
    }

    /*--------.
    | Methods |
    `--------*/

    elle::String const&
    Trait::name() const
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      return (this->_valid->name());
    }

    elle::String const&
    Trait::value() const
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      return (this->_valid->value());
    }

    void
    Trait::value(elle::String const& value) const
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      this->_valid->value(value);
    }

    /*----------.
    | Operators |
    `----------*/

    elle::Boolean
    Trait::operator ==(Trait const& other) const
    {
      if (this == &other)
        return (true);

      if (this->_type != other._type)
        return (false);

      if (this->_type == Type::valid)
        {
          ELLE_ASSERT(this->_valid != nullptr);
          ELLE_ASSERT(other._valid != nullptr);

          if ((this->_valid->name() != other._valid->name()) ||
              (this->_valid->value() != other._valid->value()))
            return (false);
        }

      return (true);
    }

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status        Trait::Dump(elle::Natural32             margin) const
    {
      elle::String      alignment(margin, ' ');

      switch (this->_type)
        {
        case Type::null:
          {
            std::cout << alignment << "[Trait] " << elle::none << std::endl;

            break;
          }
        case Type::valid:
          {
            ELLE_ASSERT(this->_valid != nullptr);

            std::cout << alignment << "[Trait]" << std::endl;

            std::cout << alignment << elle::io::Dumpable::Shift
                      << "[Name] " << this->_valid->name() << std::endl;

            std::cout << alignment << elle::io::Dumpable::Shift
                      << "[Value] " << this->_valid->value() << std::endl;

            break;
          }
        default:
          throw Exception("unknown trait type '%s'", this->_type);
        }

      return elle::Status::Ok;
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Trait::print(std::ostream& stream) const
    {
      switch (this->_type)
        {
        case Type::null:
          {
            stream << "trait(null)";
            break;
          }
        case Type::valid:
          {
            ELLE_ASSERT(this->_valid != nullptr);

            stream << "trait("
                   << this->_valid->name()
                   << ", "
                   << this->_valid->value()
                   << ")";

            break;
          }
        default:
          throw Exception("unknown trait type '%s'", this->_type);
        }
    }

    /*----------.
    | Rangeable |
    `----------*/

    elle::String const&
    Trait::symbol()
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      return (this->_valid->name());
    }

  }
}
