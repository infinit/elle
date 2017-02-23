#include <elle/system/username.hh>

#if !defined(INFINIT_WINDOWS)
# include <pwd.h>
# include <stdio.h>
# include <sys/types.h>
# include <unistd.h>
#endif

#include <elle/assert.hh>
#include <elle/os/environ.hh>

namespace elle
{
  namespace system
  {
    std::string
    username()
    {
#ifdef INFINIT_WINDOWS
      char buffer[1024];
      DWORD sz = 1024;
      GetUserName(buffer, &sz);
      return buffer;
#else
      {
        struct passwd* pw = ::getpwuid(::geteuid());
        if (pw != nullptr && pw->pw_name != nullptr)
          return pw->pw_name;
      }
      {
        auto login = getlogin();
        if (login)
          return login;
      }
      ELLE_ABORT("could not determine username");
#endif
    }
  }
}
