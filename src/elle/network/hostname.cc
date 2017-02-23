#include <elle/network/hostname.hh>

#include <cstring>
#include <unistd.h>

#include <elle/err.hh>
#include <elle/printf.hh>
#include <elle/windows.h>

namespace elle
{
  namespace network
  {
    std::string
    hostname()
    {
      char name[512];
      auto error = gethostname(name, sizeof(name));
      static std::string const message = "unable to get hostname";
      if (error == ENAMETOOLONG)
        elle::err("%s: larger than %s characters", message, sizeof(name));
      else if (error)
        elle::err("%s: %s", message, strerror(errno));
      return name;
    }
  }
}
