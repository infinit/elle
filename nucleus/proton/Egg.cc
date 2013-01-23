#include <nucleus/proton/Egg.hh>
#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Contents.hh>
#include <nucleus/proton/Clef.hh>
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
      _clef(new Clef{address, secret})
    {
    }

    Egg::Egg(Clef const* clef):
      _type(Type::permanent),
      _clef(clef)
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

    Clef const&
    Egg::clef() const
    {
      ELLE_ASSERT(this->_clef != nullptr);

      return (*this->_clef);
    }

    void
    Egg::reset(Address const& address,
               cryptography::SecretKey const& secret)
    {
      // Update the type and history according to the current type.
      switch (this->_type)
        {
        case Type::transient:
          {
            // In this case, upgrade the transient block to a permanent
            // one now that it has a valid address and secret.
            this->_type = Type::permanent;

            break;
          }
        case Type::permanent:
          {
            // In this case, the egg must keep track of the block's annals
            // by remembering the past address/secret tuples.
            this->_annals.record(std::move(this->_clef));

            break;
          }
        default:
          throw Exception("unknown egg type '%s'", this->_type);
        }

      // Regenerate the clef with the new address and secret.
      this->_clef.reset(new Clef{address, secret});
    }

    void
    Egg::lock(Reason reason)
    {
      switch (reason)
        {
        case Reason::access:
          {
            // Lock in reading.
            elle::concurrency::scheduler().current()->wait(
              this->_mutex);

            break;
          }
        case Reason::move:
          {
            // Lock in writing.
            elle::concurrency::scheduler().current()->wait(
              this->_mutex.write());

            break;
          }
        }
    }

    void
    Egg::unlock(Reason reason)
    {
      switch (reason)
        {
        case Reason::access:
          {
            // Unlock the read mutex.
            this->_mutex.release();

            break;
          }
        case Reason::move:
          {
            // Unload the write mutex.
            this->_mutex.write().release();

            break;
          }
        }
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Egg::print(std::ostream& stream) const
    {
      ELLE_ASSERT(this->_clef != nullptr);

      stream << this->_type << "(" << *this->_clef << ", "
             << this->_block.get() << ")";
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
