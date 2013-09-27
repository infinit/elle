#ifndef  ELLE_OS_ENVIRON_HH
# define ELLE_OS_ENVIRON_HH

# include <string>
# include <unordered_map>

namespace elle
{
  namespace os
  {

# ifdef environ // Prevent windows.h macros.
#  undef environ
# endif

    /// Retrieve a fresh copy of environ.
    std::unordered_map<std::string, std::string>
    environ();

  }
}

#endif

