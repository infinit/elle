#pragma once

#include <elle/reactor/network/Error.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      using Exception [[deprecated("use elle::reactor::Error instead")]]
      = Error;
    }
  }
}

