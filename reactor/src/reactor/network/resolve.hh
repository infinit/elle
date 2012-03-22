#ifndef INFINIT_REACTOR_NETWORK_RESOLVE_HH
# define INFINIT_REACTOR_NETWORK_RESOLVE_HH

# include <boost/asio.hpp>

# include <reactor/network/fwd.hh>

namespace reactor
{
  namespace network
  {
    boost::asio::ip::tcp::endpoint
    resolve_tcp(Scheduler& scheduler,
                const std::string& hostname,
                const std::string& service);
    boost::asio::ip::udp::endpoint
    resolve_udp(Scheduler& scheduler,
                const std::string& hostname,
                const std::string& service);
  }
}

#endif
