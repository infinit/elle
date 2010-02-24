#ifndef HOLE_NETWORK_LOCATION_HH
# define HOLE_NETWORK_LOCATION_HH

# include <stdint.h>
# include <QHostAddress>

namespace hole
{
  namespace network
  {
    struct Address
    {
      union Addr {
        uint32_t ipv4;
        uint8_t  ipv6[16];
      };

      enum AddrType {
        IPv4,
        IPv6,
      };

      inline void ToQHostAddress(QHostAddress * address);

      Addr     addr;
      AddrType type;
    };

    struct Location
    {
      Address  address;
      uint16_t port;
    };
  }
}

# include "hole/network/Location.hxx"

#endif /* !HOLE_NETWORK_LOCATION_HH */
