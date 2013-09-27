#include "unsetenv.hh"

#include <elle/Exception.hh>

#include <cstdlib>

#ifdef INFINIT_WINDOWS
# include <windows.h>
#endif

namespace elle
{
  namespace os
  {

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
