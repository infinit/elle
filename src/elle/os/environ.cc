#include <elle/os/environ.hh>

#if defined ELLE_MACOS
# include <crt_externs.h>
#elif defined ELLE_WINDOWS
# include <elle/windows.hh>
#else
# include <sys/types.h>
# include <unistd.h>
#endif

#include <cstdlib>
#include <cstring>

#include <elle/from-string.hh>

#ifdef environ
# undef environ
#endif

#ifdef ELLE_IOS
extern char **environ;
#endif

#include <elle/err.hh>
#include <elle/os/exceptions.hh>
#include <elle/printf.hh>

namespace elle
{
  namespace os
  {
    std::string
    setenv(std::string const& key,
           std::string const& val,
           bool overwrite)
    {
      if (!overwrite)
        if (auto value = ::getenv(key.c_str()))
          return value;

#ifdef ELLE_WINDOWS
      if (::_putenv((key + "=" + val).c_str()) != 0)
#else
      if (::setenv(key.c_str(), val.c_str(), 1) == -1)
#endif
        elle::err("cannot set %s=%s", key, val);

      return val;
    }

    void
    setenv(Environ const& env, bool overwrite)
    {
      for (auto const& e: env)
        setenv(e.first, e.second, overwrite);
    }

    std::string getenv(std::string const& key)
    {
      if (auto val = ::getenv(key.c_str()))
        return val;
      else
        throw KeyError(key);
    }

    bool
    inenv(std::string const& key)
    {
      return ::getenv(key.c_str());
    }


    Environ
    environ()
    {
      auto res = Environ{};
      auto insert = [&](char const* str) {
        auto const* cp = strchr(str, '=');
        assert(cp);
        res.emplace(std::string{str, cp}, std::string{cp+1});
      };
#ifdef ELLE_WINDOWS
      LPTCH strings = GetEnvironmentStrings();
      if (strings == nullptr)
        elle::err("cannot get environment strings");

      for (LPTSTR ptr = (LPTSTR) strings;
           *ptr != '\0';
           ptr += lstrlen(ptr) + 1)
        insert(ptr);

      FreeEnvironmentStrings(strings);
#else
# ifdef ELLE_MACOS
      char** strings = *(_NSGetEnviron());
# else
      char** strings = ::environ;
# endif
      if (strings == nullptr)
        elle::err("cannot get environment strings");

      for (char** ptr = strings; *ptr != nullptr; ++ptr)
        insert(*ptr);
#endif
      return res;
    }

    void
    unsetenv(std::string const& key)
    {
#ifdef ELLE_WINDOWS
      if (::_putenv((key + "=").c_str()) != 0)
#else
      if(::unsetenv(key.c_str()) == -1)
#endif
        elle::err("cannot unset environment variable '%s'", key);
    }
  }
}
