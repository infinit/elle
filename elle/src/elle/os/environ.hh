#pragma once

#include <string>
#include <unordered_map>

#ifdef environ // Prevent windows.h macros.
# undef environ
#endif

#include <elle/compiler.hh>

namespace elle
{
  namespace os ELLE_API
  {

    using Environ = std::unordered_map<std::string, std::string>;

    /// Retrieve a fresh copy of environ.
    Environ
    environ();

    std::string
    setenv(std::string const& key, std::string const& val, bool mode);

    /// Get a specific value from environment. If value is not found,
    /// KeyError Exception is thrown
    std::string
    getenv(std::string const& key);

    /// Get a specific value from environment. If value is not found,
    /// default_ is returned
    std::string
    getenv(std::string const& key, std::string const& default_);

    /// Whether a key is defined in environment.
    bool
    inenv(std::string const& key);

    void
    unsetenv(std::string const& key);
  }
}
