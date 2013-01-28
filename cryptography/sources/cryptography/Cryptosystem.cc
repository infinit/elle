#include <cryptography/Cryptosystem.hh>

#include <elle/Exception.hh>

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
        default:
          throw elle::Exception("unknown asymmetric cryptosystem '%s'",
                                static_cast<int>(cryptosystem));
        }

      return (stream);
    }
  }
}
