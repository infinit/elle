#include <nucleus/proton/Radix.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Root.hh>

#include <cryptography/Cipher.hh>

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Construction |
    `-------------*/

    Radix::Radix():
      _nature(Nature::empty),
    {
    }

    Radix::Radix(cryptography::Cipher const& value):
      _nature(Nature::value),
      _cipher(new cryptography::Cipher{value})
    {
    }

    Radix::Radix(Address const& address):
      _nature(Nature::block),
      _address(new Address{address})
    {
    }

    Radix::Radix(Root const& root):
      _nature(Nature::tree),
      _root(new Root{root})
    {
    }

    Radix::~Radix()
    {
      switch (this->_nature)
        {
        case Nature::empty:
          {
            break;
          }
        case Nature::value:
          {
            delete this->_cipher;

            break;
          }
        case Nature::block:
          {
            delete this->_address;

            break;
          }
        case Nature::tree:
          {
            delete this->_root;

            break;
          }
        default:
          throw Exception("unknown radix nature '%s'", this->_nature);
        }
    }

    /*--------.
    | Methods |
    `--------*/

    cryptography::Cipher const&
    Radix::value() const
    {
      ELLE_ASSERT(this->_cipher);

      return (*this->_cipher);
    }

    Address const&
    Radix::block() const
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

    /*----------.
    | Printable |
    `----------*/

    void
    Radix::print(std::ostream& stream) const
    {
      stream << this->_nature << "(";

      switch (this->_nature)
        {
        case Nature::empty:
          {
            break;
          }
        case Nature::value:
          {
            ELLE_ASSERT(this->_cipher != nullptr);

            stream << *this->_cipher;

            break;
          }
        case Nature::block:
          {
            ELLE_ASSERT(this->_address != nullptr);

            stream << *this->_address;

            break;
          }
        case Nature::tree:
          {
            ELLE_ASSERT(this->_root != nullptr);

            stream << *this->_root;

            break;
          }
        default:
          throw Exception("unknown radix nature '%s'", this->_nature);
        }
    }
  }
}
