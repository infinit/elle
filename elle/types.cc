#include <cstring>
#include <iostream>

#include <elle/types.hh>

namespace elle
{
  Null const none = Null::Nil;

  std::ostream& operator <<(std::ostream& stream, elle::Null)
  {
    return stream << "(nil)";
  }
}
