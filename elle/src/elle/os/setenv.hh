#ifndef ELLE_OS_SETENV_HH
# define ELLE_OS_SETENV_HH

#include <string>

///
/// Provides C++ wrapper around setenv() C function.
///

namespace elle
{
  namespace os
  {

    /// Sets the associated environment value.
    /// mode true implies overwrite or add.
    /// Returns val or throws key error if there was a problem.
    std::string
    setenv(std::string const& key,
           std::string const& val,
           bool mode);
  }
}

#endif
