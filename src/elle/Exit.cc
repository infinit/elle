#include <elle/Exit.hh>

#include <elle/printf.hh>

namespace elle
{
  Exit::Exit(int r)
    : Super(elle::sprintf("exit with return code %s", r))
    , _return_code(r)
  {}
}
