#pragma once

#include <iosfwd>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /// Type of network protocol supported by the reactor.
      enum class Protocol
      {
        tcp,
        utp
      };

      /// Pretty print Protocol.
      std::ostream&
      operator <<(std::ostream& out, Protocol p);
    }
  }
}
