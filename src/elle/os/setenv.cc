#include "setenv.hh"

#include <elle/Exception.hh>

#include <cstdlib>

namespace elle
{
  namespace os
  {

    class KeyError:
      public elle::Exception
    {
    public:
      KeyError(std::string const& key):
        elle::Exception("KeyError '" + key + "'")
      {}
    };

    std::string
    setenv(std::string const& key,
           std::string const& val,
           int mode)
    {
      if (::setenv(key.c_str(), val.c_str(), mode) == -1)
        throw KeyError(key);
      return val;
    }
  }
}
