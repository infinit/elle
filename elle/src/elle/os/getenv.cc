#include "getenv.hh"

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

    std::string getenv(std::string const& key)
    {
      char const* val = ::getenv(key.c_str());
      if (val == nullptr)
        throw KeyError(key);
      return std::string{val};
    }

    std::string getenv(std::string const& key,
                       std::string const& default_)
    {
      char const* val = ::getenv(key.c_str());
      if (val != nullptr)
        return std::string{val};
      else
        return default_;
    }

    bool
    in_env(std::string const& key)
    {
      char const* val = ::getenv(key.c_str());
        return (val != nullptr);
    }
  }
}
