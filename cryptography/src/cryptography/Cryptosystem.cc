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
    elle::Natural8
    Serialize<infinit::cryptography::Cryptosystem>::convert(
      infinit::cryptography::Cryptosystem const& value)
    {
      return (static_cast<elle::Natural8>(value));
    }

    infinit::cryptography::Cryptosystem
    Serialize<infinit::cryptography::Cryptosystem>::convert(
      elle::Natural8 const& representation)
    {
      return (static_cast<infinit::cryptography::Cryptosystem>(representation));
    }
  }
}
