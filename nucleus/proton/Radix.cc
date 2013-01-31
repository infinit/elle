#include <nucleus/proton/Radix.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Root.hh>

#include <cryptography/Code.hh>

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Construction |
    `-------------*/

    Radix::Radix():
      _strategy(Strategy::none)
    {
      // Manually set all the union pointers to null so as to make sure all
      // the cases are handled.
      this->_code = nullptr;
      this->_address = nullptr;
      this->_root = nullptr;
    }

    Radix::Radix(cryptography::Code const& value):
      _strategy(Strategy::value),
      _code(new cryptography::Code{value})
    {
    }

    Radix::Radix(Address const& address):
      _strategy(Strategy::block),
      _address(new Address{address})
    {
    }

    Radix::Radix(Root const& root):
      _strategy(Strategy::tree),
      _root(new Root{root})
    {
    }

    Radix::Radix(Radix const& other):
      _strategy(other._strategy)
    {
      // Manually set all the union pointers to null so as to make sure all
      // the cases are handled.
      this->_code = nullptr;
      this->_address = nullptr;
      this->_root = nullptr;

      switch (this->_strategy)
        {
        case Strategy::none:
          {
            break;
          }
        case Strategy::value:
          {
            ELLE_ASSERT(other._code != nullptr);

            this->_code = new cryptography::Code{*other._code};

            break;
          }
        case Strategy::block:
          {
            ELLE_ASSERT(other._address != nullptr);

            this->_address = new Address{*other._address};

            break;
          }
        case Strategy::tree:
          {
            ELLE_ASSERT(other._root != nullptr);

            this->_root = new Root{*other._root};

            break;
          }
        default:
          throw Exception("unknown radix strategy '%s'", this->_strategy);
        }
    }

    Radix::~Radix()
    {
      switch (this->_strategy)
        {
        case Strategy::none:
          {
            break;
          }
        case Strategy::value:
          {
            delete this->_code;

            break;
          }
        case Strategy::block:
          {
            delete this->_address;

            break;
          }
        case Strategy::tree:
          {
            delete this->_root;

            break;
          }
        default:
          throw Exception("unknown radix strategy '%s'", this->_strategy);
        }
    }

    /*--------.
    | Methods |
    `--------*/

    elle::Boolean
    Radix::empty() const
    {
      return (this->_strategy == Strategy::none);
    }

    cryptography::Code const&
    Radix::value() const
    {
      ELLE_ASSERT(this->_strategy == Strategy::value);
      ELLE_ASSERT(this->_code);

      return (*this->_code);
    }

    Address const&
    Radix::block() const
    {
      ELLE_ASSERT(this->_strategy == Strategy::block);
      ELLE_ASSERT(this->_address);

      return (*this->_address);
    }

    Root const&
    Radix::tree() const
    {
      ELLE_ASSERT(this->_strategy == Strategy::tree);
      ELLE_ASSERT(this->_root);

      return (*this->_root);
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Radix::print(std::ostream& stream) const
    {
      stream << this->_strategy;

      switch (this->_strategy)
        {
        case Strategy::none:
          {
            break;
          }
        case Strategy::value:
          {
            ELLE_ASSERT(this->_code != nullptr);

            stream << "(" << *this->_code << ")";

            break;
          }
        case Strategy::block:
          {
            ELLE_ASSERT(this->_address != nullptr);

            stream << "(" << *this->_address << ")";

            break;
          }
        case Strategy::tree:
          {
            ELLE_ASSERT(this->_root != nullptr);

            stream << "(" << *this->_root << ")";

            break;
          }
        default:
          throw Exception("unknown radix strategy '%s'", this->_strategy);
        }
    }
  }
}
