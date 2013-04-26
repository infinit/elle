#pragma once
#ifndef HOLEPUNCHER_LFFVGYPW
# define HOLEPUNCHER_LFFVGYPW

#include <elle/network/Locus.hh>
#include <elle/attribute.hh>

#include <reactor/scheduler.hh>
#include <reactor/network/udp-socket.hh>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <algorithm>
#include <string>

namespace reactor
{
  namespace nat
  {

    inline boost::posix_time::seconds
    operator "" _sec(const char* str)
    {
      return boost::posix_time::seconds{std::stoi(str)};
    }
 
    class Hole
    {
    private:
      std::unique_ptr<reactor::network::UDPSocket>      _handle;
      boost::asio::ip::udp::endpoint                    _longinus;

      ELLE_ATTRIBUTE_R(boost::asio::ip::udp::endpoint, public_endpoint);
    public:
      std::unique_ptr<reactor::network::UDPSocket>
      handle();

    public:
      Hole() = delete;
      Hole(Hole const& hole) = delete;
      Hole& operator =(Hole const& other) = delete;
      Hole& operator =(Hole&& other) = delete;
      Hole(Hole&& hole);
      ~Hole() = default;

      Hole(reactor::Scheduler &sched,
           boost::asio::ip::udp::endpoint const& longinus,
           int local_port = 0);
    private:
      boost::asio::ip::udp::endpoint
      _punch(unsigned short port, boost::posix_time::seconds timeout = 10_sec);
    };

    class NAT
    {
    public:

    public:
      NAT(reactor::Scheduler &s);
      ~NAT() = default;

    public:
      Hole
      punch(std::string const &hostname,
            int port,
            int local_port = 0);

    private:
      reactor::Scheduler&         sched;
    };

  } /* nat */
} /* elle */

#endif /* end of include guard: HOLEPUNCHER_LFFVGYPW */
