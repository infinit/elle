#include "unsetenv.hh"

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

    void
    unsetenv(std::string const& key)
    {
      if(::unsetenv(key.c_str()) == -1)
        throw KeyError(key);
    }

    std::string
    unsetenv(std::string const& key,
             std::string const& default_)
    {
      if (::unsetenv(key.c_str()) == -1)
        return key;
      else
        return default_;
    }

  }
}
