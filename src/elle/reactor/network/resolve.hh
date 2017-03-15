#pragma once

#include <vector>

#include <boost/asio.hpp>
#include <boost/variant.hpp>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /// A port number, or a service name.
      using Port = boost::variant<int, std::string>;
      
      std::vector<boost::asio::ip::tcp::endpoint>
      resolve_tcp(const std::string& hostname,
                  const Port& service,
                  bool ipv4_only = true);

      std::vector<boost::asio::ip::udp::endpoint>
      resolve_udp(const std::string& hostname,
                  const Port& service,
                  bool ipv4_only = true);
    }
  }
}
