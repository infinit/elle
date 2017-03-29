#pragma once

#include <vector>

#include <boost/asio.hpp>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      struct ResolveOptions
      {
        ResolveOptions(bool v4_only = false, int num = 10)
          : ipv4_only{v4_only}
          , num_attempts{num}
        {}
        ResolveOptions(int num)
          : num_attempts{num}
        {}
        bool ipv4_only;
        int num_attempts;
      };

      /// FIXME: fix these signatures.  They should be simple
      /// overloads, but literal strings have a tendency to be bound
      /// to Booleans rather than std::strings.

      /// Resolve TCP hostname.
      std::vector<boost::asio::ip::tcp::endpoint>
      resolve_tcp(std::string const& hostname,
                  int port,
                  ResolveOptions = {});

      std::vector<boost::asio::ip::tcp::endpoint>
      resolve_tcp(std::string const& hostname,
                  std::string const& service,
                  ResolveOptions = {});

      /// Resolve "hostname:port" or "hostname:service" strings for TCP.
      std::vector<boost::asio::ip::tcp::endpoint>
      resolve_tcp_repr(std::string const& repr,
                       ResolveOptions = {});

      /// Resolve UDP hostname.
      std::vector<boost::asio::ip::udp::endpoint>
      resolve_udp(std::string const& hostname,
                  int port,
                  ResolveOptions = {});

      std::vector<boost::asio::ip::udp::endpoint>
      resolve_udp(std::string const& hostname,
                  std::string const& service,
                  ResolveOptions = {});

      /// Resolve "hostname:port" or "hostname:service" strings for UDP.
      std::vector<boost::asio::ip::udp::endpoint>
      resolve_udp_repr(std::string const& repr,
                       ResolveOptions = {});
    }
  }
}
