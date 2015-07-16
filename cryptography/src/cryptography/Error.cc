#include <cryptography/Error.hh>

namespace infinit
{
  namespace cryptography
  {
    /*-------------.
    | Construction |
    `-------------*/

    Error::Error(elle::String const& message):
      elle::Error(message)
    {}
  }
}
