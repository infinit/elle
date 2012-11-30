#include <nucleus/proton/Radix.hh>
#include <nucleus/proton/Handle.hh>
#include <nucleus/proton/Value.hh>
#include <nucleus/proton/Tree.hh>

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

    /*--------.
    | Methods |
    `--------*/

    Value const&
    Radix::value() const
    {
      ELLE_ASSERT(this->_value);

      return (*this->_value);
    }

    Value&
    Radix::value()
    {
      ELLE_ASSERT(this->_value);

      return (*this->_value);
    }

    Handle const&
    Radix::block() const
    {
      ELLE_ASSERT(this->_block);

      return (*this->_block);
    }

    Handle&
    Radix::block()
    {
      ELLE_ASSERT(this->_block);

      return (*this->_block);
    }

    Tree const&
    Radix::tree() const
    {
      ELLE_ASSERT(this->_tree);

      return (*this->_tree);
    }

    Tree&
    Radix::tree()
    {
      ELLE_ASSERT(this->_tree);

      return (*this->_tree);
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
            ELLE_ASSERT(this->_block != nullptr);

            stream << *this->_block;

            break;
          }
        case Mode::tree:
          {
            ELLE_ASSERT(this->_tree != nullptr);

            stream << *this->_tree;

            break;
          }
        default:
          throw Exception("unknown radix mode '%s'", this->_mode);
        }
    }

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Radix::Mode const mode)
    {
      switch (mode)
        {
        case Radix::Mode::empty:
          {
            stream << "empty";
            break;
          }
        case Radix::Mode::value:
          {
            stream << "value";
            break;
          }
        case Radix::Mode::block:
          {
            stream << "block";
            break;
          }
        case Radix::Mode::tree:
          {
            stream << "tree";
            break;
          }
        default:
          {
            throw Exception("unknown mode: '%s'", static_cast<int>(mode));
          }
        }

      return (stream);
    }
  }
}
