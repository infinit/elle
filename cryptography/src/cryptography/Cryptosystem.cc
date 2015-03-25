#include <cryptography/Cryptosystem.hh>
#include <cryptography/Exception.hh>

#include <elle/printf.hh>

#include <ostream>

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
          throw Exception(elle::sprintf("unknown asymmetric cryptosystem '%s'",
                                        static_cast<int>(cryptosystem)));
      }

      return (stream);
    }
  }
}
