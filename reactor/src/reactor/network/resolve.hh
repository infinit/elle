#ifndef INFINIT_REACTOR_NETWORK_RESOLVE_HH
# define INFINIT_REACTOR_NETWORK_RESOLVE_HH

# include <boost/asio.hpp>

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

#endif
