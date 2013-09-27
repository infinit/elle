#include "setenv.hh"

#include <elle/Exception.hh>

#include <stdlib.h>

#ifdef INFINIT_WINDOWS
# include <windows.h>
#endif

namespace elle
{
  namespace os
  {

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

  }
}
