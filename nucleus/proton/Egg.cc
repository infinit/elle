#include <nucleus/proton/Egg.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Contents.hh>
#include <nucleus/proton/Clef.cc>
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
      _clef(new Clef{address, secret}),
      _block(nullptr)
    {
    }

    Egg::Egg(Clef* clef):
      _type(Type::permanent),
      _clef(clef),
      _block(nullptr)
    {
    }

    Egg::Egg(Contents* block,
             Address const& address,
             cryptography::SecretKey const& secret):
      _type(Type::transient),
      _clef(new Clef{address, secret}),
      _block(block)
    {
    }

    Egg::~Egg()
    {
      delete this->_clef;
      delete this->_block;
    }

    /*--------.
    | Methods |
    `--------*/

    Address const&
    Egg::address() const
    {
      ELLE_ASSERT(this->_clef != nullptr);

      return (this->_clef->address());
    }

    cryptography::SecretKey const&
    Egg::secret() const
    {
      ELLE_ASSERT(this->_clef != nullptr);

      return (this->_clef->secret());
    }

    void
    Egg::reset(Address const& address,
               cryptography::SecretKey const& secret)
    {
      // Regenerate the clef with the new address and secret.
      delete this->_clef;
      this->_clef = nullptr;
      this->_clef = new Clef{address, secret};
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
    | Printable |
    `----------*/

    void
    Egg::print(std::ostream& stream) const
    {
      ELLE_ASSERT(this->_clef != nullptr);

      stream << this->_type << "(" << *this->_clef << ")";
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
