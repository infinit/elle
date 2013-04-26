#ifndef PROTOCOLS_HH
# define PROTOCOLS_HH

#include <iosfwd>

namespace reactor
{
  namespace network
  {
    enum class Protocol
    {
      tcp,
      udt,
    };

    std::ostream&
    operator <<(std::ostream& out, Protocol p);
  }
}

#endif
