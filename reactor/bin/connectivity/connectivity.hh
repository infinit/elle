#ifndef REACTOR_CONNECTIVITY_CONNECTIVITY_HH
# define REACTOR_CONNECTIVITY_CONNECTIVITY_HH

#include <reactor/network/buffer.hh>
#include <reactor/network/rdv.hh>
#include <reactor/network/resolve.hh>
#include <reactor/network/tcp-server.hh>
#include <reactor/network/tcp-socket.hh>
#include <reactor/network/udp-server.hh>
#include <reactor/network/udp-socket.hh>
#include <reactor/network/utp-socket.hh>
#include <reactor/scheduler.hh>

namespace reactor
{
  namespace connectivity
  {
    struct Result
    {
      Result(std::string res,
             uint16_t local_port)
        : host(res.substr(0, res.find(":")))
        , remote_port(std::stoi(res.substr(res.find(":") + 1)))
        , local_port(local_port)
      {
      }

      std::string host;
      uint16_t remote_port;
      uint16_t local_port;
    };

    static
    Result
    tcp(std::string const& host, int port)
    {
      reactor::network::TCPSocket s(host, port, 5_sec);
      s << "foo" << std::endl;
      std::string line;
      std::getline(s, line);
      auto p = line.find(' ');
      if (line.substr(p+1) != "foo")
        throw std::runtime_error("Unexpected output: " + line);
      return {
        line.substr(0, p), s.local_endpoint().port()
      };
    }

    static
    Result
    udp(std::string const& host, int port)
    {
      reactor::network::UDPSocket s;
      s.close();
      s.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0));
      auto server_ep = reactor::network::resolve_udp(host, std::to_string(port));
      s.send_to(std::string("foo\n"), server_ep);
      boost::asio::ip::udp::endpoint ep;
      char buffer[5000];
      auto sz = s.receive_from(reactor::network::Buffer(buffer, 5000), ep, 5_sec);
      std::string line(buffer, sz);
      auto p = line.find(' ');
      if (line.substr(p+1) != "foo\n")
        throw std::runtime_error("Unexpected output: " + line);
      return {line.substr(0, p), s.local_endpoint().port()};
    }

    static
    std::string
    nat(std::string const& host, int port)
    {
      // check cone-nat by comparing our endpoints from rdv server and connectivity server
      reactor::network::RDVSocket socket;
      socket.close();
      socket.bind(
        boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port));
      // we must be polling for rdv_connect to work
      std::string result;
      bool success = true;
      reactor::Thread poller("poll", [&]
                             {
                               while (true)
                               {
                                 elle::Buffer buf;
                                 buf.size(5000);
                                 boost::asio::ip::udp::endpoint ep;
                                 int sz = socket.receive_from(reactor::network::Buffer(buf.mutable_contents(),
                                                                                       buf.size()), ep);
                                 buf.size(sz);
                                 std::string line(buf.string());
                                 auto p = line.find(' ');
                                 if (line.substr(p+1) != "foo\n")
                                 {
                                   success = false;
                                   result = "Unexpected output: " + line;
                                 }
                                 else
                                 {
                                   bool cone = (line.substr(0, p) == elle::sprintf("%s", socket.public_endpoint()));
                                   result = "UDP-RDV OK: ";
                                   if (cone)
                                     result += "CONE NAT";
                                   else
                                     result += elle::sprintf("NOT_CONE NAT %s %s", line.substr(0, p),
                                                             socket.public_endpoint());
                                 }
                                 break;
                               }
                             });
      elle::SafeFinally spoll([&] {
          poller.terminate_now();
        });
      socket.rdv_connect("connectivity-client-udp",
                         reactor::network::resolve_udp("rdv.infinit.sh", "7890"),
                         5_sec);
      auto ep = reactor::network::resolve_udp(host, std::to_string(port));
      socket.send_to(std::string("foo\n"), ep);
      reactor::wait(poller);
      spoll.abort();
      if (!success)
        throw std::runtime_error(result);
      return result;
    }

    static
    Result
    rdv_utp(std::string const& host, int port)
    {
      reactor::network::UTPServer server;
      server.listen(0);
      server.rdv_connect("connectivity", "rdv.infinit.sh:7890", 5_sec);
      reactor::network::UTPSocket s(server);
      s.connect("connectivity-server", {}, 5_sec);
      s << "foo" << std::endl;
      elle::Buffer reply = s.read_until("\n", 5_sec);
      std::string line = reply.string();
      auto p = line.find(' ');
      if (line.substr(p+1) != "foo\n")
        throw std::runtime_error("Unexpected output: " + line);
      return {line.substr(0, p), server.local_endpoint().port()};
    }
  }
}

#endif
