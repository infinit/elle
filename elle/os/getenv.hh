#ifndef  ELLE_OS_GETENV_HH
# define ELLE_OS_GETENV_HH

# include <string>

///
/// Provides C++ wrapper around getenv() C function.
///

namespace elle
{
  namespace os
  {
    /// Returns the associated environment value or throw when the key is not
    /// found.
    std::string getenv(std::string const& key);

    /// Return the associated environment value or the provided default value
    /// when the key is not found.
    std::string getenv(std::string const& key,
                       std::string const& default_);

  }
}

#endif

