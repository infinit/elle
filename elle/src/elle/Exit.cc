#include <elle/Exit.hh>

#include <elle/printf.hh>

namespace elle
{
  Exit::Exit(int return_code)
    : Super(elle::sprintf("exit with return code %s", return_code))
    , _return_code(return_code)
  {}
}
