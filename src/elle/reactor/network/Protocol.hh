#pragma once

#include <iosfwd>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      enum class Protocol
      {
        tcp,
        utp
      };

      std::ostream&
      operator <<(std::ostream& out, Protocol p);
    }
  }
}
