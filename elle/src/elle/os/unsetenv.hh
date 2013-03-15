#ifndef ELLE_OS_UNSETENV_HH
#define ELLE_OS_UNSETENV_HH

#include <string>

///
/// Provides C++ wrapper around unsetenv() C function.
///

namespace elle
{
  namespace os
  {
    /// Unset the associated environment value or throw when the key is not
    /// found.
    void
    unsetenv(std::string const& key);

    /// Unset the associated environment value or the provided default value
    /// when the key is not found.
    std::string
    unsetenv(std::string const& key,
             std::string const& default_);
  }
}

#endif
