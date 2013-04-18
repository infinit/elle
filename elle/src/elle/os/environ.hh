#ifndef  ELLE_OS_ENVIRON_HH
# define ELLE_OS_ENVIRON_HH

# include <string>
# include <unordered_map>

namespace elle
{
  namespace os
  {

    /// Retrieve a fresh copy of environ.
    std::unordered_map<std::string, std::string>
    environ();

  }
}

#endif

