#include <cryptography/Seed.hh>
#include <cryptography/PrivateKey.hh>
#include <cryptography/PublicKey.hh>
#include <cryptography/KeyPair.hh>
#include <cryptography/random.hh>

#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.cryptography.Seed");

namespace infinit
{
  namespace cryptography
  {
    /*---------------.
    | Static Methods |
    `---------------*/

    Seed
    Seed::generate(KeyPair const& pair)
    {
      ELLE_TRACE_FUNCTION(pair);

      Seed seed(random::generate<elle::Buffer>(pair.K().size()));

      return (seed);
    }

    /*-------------.
    | Construction |
    `-------------*/

    Seed::Seed(elle::Buffer&& buffer):
      _buffer(std::move(buffer))
    {
    }

    Seed::Seed(Seed&& other):
      _buffer(std::move(other._buffer))
    {
    }

    /*----------.
    | Operators |
    `----------*/

    elle::Boolean
    Seed::operator ==(Seed const& other) const
    {
      if (this == &other)
        return (true);

      return (this->_buffer == other._buffer);
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Seed::print(std::ostream& stream) const
    {
      stream << this->_buffer;
    }
  }
}
