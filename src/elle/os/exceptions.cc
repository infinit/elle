#include <elle/os/exceptions.hh>

namespace elle
{
  namespace os
  {
    KeyError::KeyError(std::string const& key):
      elle::Exception("KeyError '" + key + "'")
    {}
  }
}