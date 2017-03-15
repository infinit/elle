#pragma once

#include <boost/asio.hpp>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      boost::asio::ip::tcp::endpoint
      resolve_tcp(const std::string& hostname,
                  const std::string& service,
                  bool ipv4_only = true);
      boost::asio::ip::udp::endpoint
      resolve_udp(const std::string& hostname,
                  const std::string& service,
                  bool ipv4_only = true);
    }
  }
}
