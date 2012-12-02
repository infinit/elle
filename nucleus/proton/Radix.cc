#include <nucleus/proton/Radix.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Value.hh>
#include <nucleus/proton/Root.hh>

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Construction |
    `-------------*/

    Radix::Radix():
      _mode(Mode::empty),
      _value(nullptr)
    {
    }

    Radix::Radix(T* value):
      _mode(Mode::value),
      _value(value)
    {
    }

    Radix::Radix(Address const& address):
      _mode(Mode::block),
      _address(new Address{address})
    {
    }

    Radix::Radix(Root const& root):
      _mode(Mode::tree),
      _root(new Root{root})
    {
    }

    Radix::~Radix()
    {
      switch (this->_mode)
        {
        case Mode::empty:
          {
            break;
          }
        case Mode::value:
          {
            break;
          }
        case Mode::block:
          {
            delete this->_address;

            break;
          }
        case Mode::tree:
          {
            delete this->_root;

            break;
          }
        default:
          throw Exception("unknown radix mode '%s'", this->_mode);
        }
    }

    /*--------.
    | Methods |
    `--------*/

    Value const*
    Radix::value() const
    {
      ELLE_ASSERT(this->_value);

      return (this->_value);
    }

    Value*
    Radix::value()
    {
      ELLE_ASSERT(this->_value);

      return (this->_value);
    }

    Address const&
    Radix::block() const
    {
      ELLE_ASSERT(this->_address);

      return (*this->_address);
    }

    Address&
    Radix::block()
    {
      ELLE_ASSERT(this->_address);

      return (*this->_address);
    }

    Root const&
    Radix::tree() const
    {
      ELLE_ASSERT(this->_root);

      return (*this->_root);
    }

    Root&
    Radix::tree()
    {
      ELLE_ASSERT(this->_root);

      return (*this->_root);
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Radix::print(std::ostream& stream) const
    {
      stream << this->_mode << "(";

      switch (this->_mode)
        {
        case Mode::empty:
          {
            break;
          }
        case Mode::value:
          {
            ELLE_ASSERT(this->_value != nullptr);

            stream << *this->_value;

            break;
          }
        case Mode::block:
          {
            ELLE_ASSERT(this->_address != nullptr);

            stream << *this->_address;

            break;
          }
        case Mode::tree:
          {
            ELLE_ASSERT(this->_root != nullptr);

            stream << *this->_root;

            break;
          }
        default:
          throw Exception("unknown radix mode '%s'", this->_mode);
        }
    }
  }
}
