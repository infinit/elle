#include "getenv.hh"

#include <elle/os/exceptions.hh>

#include <cstdlib>

namespace elle
{
  namespace os
  {
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
