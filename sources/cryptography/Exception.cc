#include <cryptography/Exception.hh>

namespace infinit
{
  namespace cryptography
  {
    /*-------------.
    | Construction |
    `-------------*/
    Exception::Exception(elle::String const& message):
      elle::Exception(message)
    {}
  }
}
