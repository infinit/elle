#include <ostream>

#include <elle/printf.hh>

#include <cryptography/Cryptosystem.hh>
#include <cryptography/Error.hh>

namespace infinit
{
  namespace cryptography
  {
    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Cryptosystem const cryptosystem)
    {
      switch (cryptosystem)
      {
        case Cryptosystem::rsa:
        {
          stream << "rsa";
          break;
        }
        case Cryptosystem::dh:
        {
          stream << "dh";
          break;
        }
        case Cryptosystem::dsa:
        {
          stream << "dsa";
          break;
        }
        case Cryptosystem::ec:
        {
          stream << "ec";
          break;
        }
        default:
          throw Error(elle::sprintf("unknown asymmetric cryptosystem '%s'",
                                    static_cast<int>(cryptosystem)));
      }

      return (stream);
    }
  }
}

/*--------------.
| Serialization |
`--------------*/

namespace elle
{
  namespace serialization
  {
    uint8_t
    Serialize<infinit::cryptography::Cryptosystem>::convert(
      infinit::cryptography::Cryptosystem const& value)
    {
      return (static_cast<uint8_t>(value));
    }

    infinit::cryptography::Cryptosystem
    Serialize<infinit::cryptography::Cryptosystem>::convert(
      uint8_t const& representation)
    {
      return (static_cast<infinit::cryptography::Cryptosystem>(representation));
    }
  }
}
