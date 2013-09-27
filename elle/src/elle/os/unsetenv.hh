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

  }
}

#endif
