#include <iostream>

#include <elle/assert.hh>

namespace elle
{
  void
  unreachable()
  {
    abort("unreachable code reached");
  }

  void
  abort(std::string const& message)
  {
    // FIXME: argv[0]
    std::cerr << "fatal error: " << message << std::endl;
    std::abort();
  }
}
