#include <nucleus/proton/Egg.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Contents.hh>
#include <nucleus/Exception.hh>

#include <cryptography/SecretKey.hh>

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Construction |
    `-------------*/

    Egg::Egg(Address const& address,
             cryptography::SecretKey const& secret):
      _type(Type::permanent),
      _address(new Address{address}),
      _secret(new cryptography::SecretKey{secret}),
      _block(nullptr)
    {
    }

    Egg::Egg(Contents* block,
             Address const& address,
             cryptography::SecretKey const& secret):
      _type(Type::transient),
      _address(new Address{address}),
      _secret(new cryptography::SecretKey{secret}),
      _block(block)
    {
    }

    Egg::~Egg()
    {
      delete this->_address;
      delete this->_secret;
      delete this->_block;
    }

    /*--------.
    | Methods |
    `--------*/

    Address const&
    Egg::address() const
    {
      ELLE_ASSERT(this->_address != nullptr);

      return (*this->_address);
    }

    cryptography::SecretKey const&
    Egg::secret() const
    {
      ELLE_ASSERT(this->_secret != nullptr);

      return (*this->_secret);
    }

    void
    Egg::reset(Address const& address,
               cryptography::SecretKey const& secret)
    {
      delete this->_address;
      this->_address = nullptr;
      this->_address = new Address{address};

      delete this->_secret;
      this->_secret = nullptr;
      this->_secret = new cryptography::SecretKey{secret};
    }

    void
    Egg::lock()
    {
      elle::concurrency::scheduler().current()->wait(this->_mutex.write());
    }

    void
    Egg::unlock()
    {
      this->_mutex.write().release();
    }

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Egg::Type const type)
    {
      switch (type)
        {
        case Egg::Type::transient:
          {
            stream << "transient";
            break;
          }
        case Egg::Type::permanent:
          {
            stream << "permanent";
            break;
          }
        default:
          {
            throw Exception("unknown egg type: '%s'", static_cast<int>(type));
          }
        }

      return (stream);
    }
  }
}
