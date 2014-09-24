#include <unistd.h>

#include <elle/Error.hh>
#include <elle/network/hostname.hh>
#include <elle/printf.hh>

namespace elle
{
  namespace network
  {
    elle::String
    hostname()
    {
      char name[512];
      auto error = gethostname(name, sizeof(name));
      static std::string const message = "unable to get hostname";
      if (error == ENAMETOOLONG)
        throw elle::Error
          (elle::sprintf("%s: larger than %s characters", message, sizeof(name)));
      else if (error)
        throw elle::Error(elle::sprintf("%s: %s", message, strerror(errno)));
      return elle::String(name);
    }
  }
}
