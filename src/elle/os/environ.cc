#include "environ.hh"

#ifdef INFINIT_MACOSX
# include <crt_externs.h>
#else
# include <environ.h>
#endif

namespace elle
{
  namespace os
  {

    std::unordered_map<std::string, std::string>
    environ()
    {
#ifdef INFINIT_MACOSX
      char** environ = *(_NSGetEnviron());
#endif
      std::unordered_map<std::string, std::string> env;
      if (environ == nullptr)
        return env;

      for (char** envp = environ; *envp != nullptr; ++envp)
        {
          char* str = *envp;
          if (*str == '\0')
            continue;
          unsigned int i = 0;
          while (str[i] != '\0' && str[i] != '=')
            i++;
          if (str[i] == '=')
            env[std::string{str, 0, i}] = &str[i + 1];
          else // no '=' found.
            env[str] = "";
        }
      return env;
    }

  }
}

