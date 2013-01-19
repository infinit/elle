#include <nucleus/neutron/Trait.hh>
#include <nucleus/Exception.hh>

#include <elle/serialize/footprint.hh>

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
      _valid(nullptr),
      _footprint(0)
    {
    }

    Trait::Trait(elle::String const& name,
                 elle::String const& value):
      _type(Type::valid),
      _valid(new Valid(name, value)),
      _footprint(0)
    {
      // Compute the initial footprint.
      this->_footprint = elle::serialize::footprint(*this);
    }

    Trait::Trait(Trait const& other):
      _type(other._type),
      _valid(nullptr),
      _footprint(other._footprint)
    {
      if (other._valid != nullptr)
        this->_valid = new Valid(other._valid->name(),
                                 other._valid->value());
    }

    Trait::Trait(Type const type):
      _type(type),
      _valid(nullptr),
      _footprint(0)
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

      // Compute the initial footprint.
      this->_footprint = elle::serialize::footprint(*this);
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

    void
    Trait::name(elle::String const& name)
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      // Substract the current name's footprint.
      ELLE_ASSERT(this->_footprint >= elle::serialize::footprint(name));
      this->_footprint -= elle::serialize::footprint(name);

      // Update the name;
      this->_valid->name(name);

      // Add the new name's footprint.
      this->_footprint += elle::serialize::footprint(name);
    }

    elle::String const&
    Trait::value() const
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      return (this->_valid->value());
    }

    void
    Trait::value(elle::String const& value)
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      // Substract the current value's footprint.
      ELLE_ASSERT(this->_footprint >=
                  elle::serialize::footprint(this->_valid->value()));
      this->_footprint -=
        elle::serialize::footprint(this->_valid->value());

      // Update the value;
      this->_valid->value(value);

      // Add the new value's footprint.
      this->_footprint += elle::serialize::footprint(value);
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
            stream << "null";
            break;
          }
        case Type::valid:
          {
            ELLE_ASSERT(this->_valid != nullptr);

            stream << "\""
                   << this->_valid->name()
                   << "\""
                   << ": "
                   << "\""
                   << this->_valid->value()
                   << "\"";

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
    Trait::symbol() const
    {
      ELLE_ASSERT(this->_type == Type::valid);
      ELLE_ASSERT(this->_valid != nullptr);

      return (this->_valid->name());
    }

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Trait::Type const type)
    {
      switch (type)
        {
        case Trait::Type::null:
          {
            stream << "null";
            break;
          }
        case Trait::Type::valid:
          {
            stream << "valid";
            break;
          }
        default:
          {
            throw Exception("unknown trait type: '%s'",
                            static_cast<int>(type));
          }
        }

      return (stream);
    }
  }
}
