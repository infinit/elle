#include <elle/os/environ.hh>

#include <elle/Exception.hh>
#include <elle/os/exceptions.hh>

#ifdef INFINIT_MACOSX
# include <crt_externs.h>
#elif defined(INFINIT_WINDOWS)
# include <windows.h>
#else
# include <sys/types.h>
# include <unistd.h>
#endif

#include <cstdlib>

#ifdef environ
# undef environ
#endif

#ifdef INFINIT_IOS
extern char **environ;
#endif

namespace elle
{
  namespace os
  {
    static
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

    std::string
    setenv(std::string const& key,
           std::string const& val,
           bool mode)
    {
      if (mode == false)
        if (char const* old_value = ::getenv(key.c_str()))
          return std::string{old_value};

#ifdef INFINIT_WINDOWS
      if (::_putenv((key + "=" + val).c_str()) != 0)
#else
      if (::setenv(key.c_str(), val.c_str(), 1) == -1)
#endif
        throw elle::Exception{
          "couldn't set " + key + "=" + val};

      return val;
    }

    std::string getenv(std::string const& key)
    {
      char const* val = ::getenv(key.c_str());
      if (val == nullptr)
        throw KeyError(key);
      return std::string{val};
    }

    std::string getenv(std::string const& key,
                       std::string const& default_)
    {
      char const* val = ::getenv(key.c_str());
      if (val != nullptr)
        return std::string{val};
      else
        return default_;
    }

    bool
    inenv(std::string const& key)
    {
      char const* val = ::getenv(key.c_str());
      return val != nullptr;
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

    void
    unsetenv(std::string const& key)
    {
#ifdef INFINIT_WINDOWS
      if (::_putenv((key + "=").c_str()) != 0)
#else
      if(::unsetenv(key.c_str()) == -1)
#endif
        throw elle::Exception{
          "couldn't unset environment variable '" + key + "'"};
    }

  }
}


