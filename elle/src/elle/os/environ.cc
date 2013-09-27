#include "environ.hh"

#include <elle/Exception.hh>

#ifdef INFINIT_MACOSX
# include <crt_externs.h>
#elif defined(INFINIT_WINDOWS)
# include <windows.h>
#else
# include <sys/types.h>
# include <unistd.h>
#endif

namespace elle
{
  namespace os
  {

#ifdef environ
# undef environ
#endif

    namespace
    {

      std::pair<std::string, std::string>
      split_env_var(char const* str)
      {
        unsigned int i = 0;
        while (str[i] != '\0' && str[i] != '=')
          i++;
        if (str[i] == '=')
          return {{str, 0, i}, &str[i + 1]};
        else // no '=' found.
          return {str, ""};
      }

    }

    std::unordered_map<std::string, std::string>
    environ()
    {
      std::unordered_map<std::string, std::string> env;
#ifdef INFINIT_WINDOWS
      LPTCH strings = GetEnvironmentStrings();
      if (strings == nullptr)
        throw elle::Exception{"couldn't get environment strings"};

      for (LPTSTR ptr = (LPTSTR) strings;
           *ptr != '\0';
           ptr += lstrlen(ptr) + 1)
        env.insert(split_env_var(ptr));

      FreeEnvironmentStrings(strings);
#else
# ifdef INFINIT_MACOSX
      char** strings = *(_NSGetEnviron());
# else
      char** strings = ::environ;
# endif
      if (strings == nullptr)
        throw elle::Exception{"couldn't get environment strings"};

      for (char** ptr = strings; *ptr != nullptr; ++ptr)
        env.insert(split_env_var(*ptr));
#endif
      return env;
    }

  }
}
