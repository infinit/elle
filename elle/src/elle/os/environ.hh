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

    /// Return if a key is defined in environment var. 
    bool
    inenv(std::string const& key);    

    void
    unsetenv(std::string const& key);

  }
}

#endif

