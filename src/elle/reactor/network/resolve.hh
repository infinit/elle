#pragma once

#include <vector>

#include <boost/asio.hpp>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /// FIXME: fix these signatures.  They should be simple
      /// overloads, but literal strings have a tendency to be bound
      /// to Booleans rather than std::strings.

      /// Resolve TCP hostname.
      std::vector<boost::asio::ip::tcp::endpoint>
      resolve_tcp(std::string const& hostname,
                  int port,
                  bool ipv4_only = true);

      std::vector<boost::asio::ip::tcp::endpoint>
      resolve_tcp(std::string const& hostname,
                  std::string const& service,
                  bool ipv4_only = true);

      /// Resolve "hostname:port" or "hostname:service" strings for TCP.
      std::vector<boost::asio::ip::tcp::endpoint>
      resolve_tcp_repr(std::string const& repr,
                       bool ipv4_only = true);

      /// Resolve UDP hostname.
      std::vector<boost::asio::ip::udp::endpoint>
      resolve_udp(std::string const& hostname,
                  int port,
                  bool ipv4_only = true);

      std::vector<boost::asio::ip::udp::endpoint>
      resolve_udp(std::string const& hostname,
                  std::string const& service,
                  bool ipv4_only = true);

      /// Resolve "hostname:port" or "hostname:service" strings for UDP.
      std::vector<boost::asio::ip::udp::endpoint>
      resolve_udp_repr(std::string const& repr,
                       bool ipv4_only = true);
    }
  }
}
