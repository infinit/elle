#include <elle/reactor/connectivity/connectivity.hh>
#include <elle/reactor/network/rdv-socket.hh>
#include <elle/reactor/network/rdv.hh>
#include <elle/reactor/network/resolve.hh>
#include <elle/reactor/network/TCPServer.hh>
#include <elle/reactor/network/TCPSocket.hh>
#include <elle/reactor/network/udp-socket.hh>
#include <elle/reactor/network/utp-server.hh>
#include <elle/reactor/network/utp-socket.hh>
#include <elle/reactor/scheduler.hh>

namespace
{
  /// "www.infinit.sh:80" -> pair("www.infinit.sh", "80").
  // duplicate from reactor/network/resolve.cc, should be put
  // somewhere.
  auto
  host_port(std::string const& repr)
  {
    auto const sep = repr.find_last_of(':');
    if (sep == std::string::npos || sep == repr.length())
      elle::err("invalid endpoint: %s", repr);
    return std::make_pair(repr.substr(0, sep), repr.substr(sep + 1));
  }
}

namespace elle
{
  namespace reactor
  {
    namespace connectivity
    {
      Result::Result(std::string res,
                     uint16_t local_port)
        : local_port(local_port)
      {
        auto hp = host_port(res);
        host = hp.first;
        remote_port = std::stoi(hp.second);
      }

      Result
      tcp(std::string const& host, int port)
      {
        elle::reactor::network::TCPSocket s(host, port, 5_sec);
        s << "foo" << std::endl;
        std::string line;
        std::getline(s, line);
        auto p = line.find(' ');
        if (line.substr(p+1) != "foo")
          throw std::runtime_error("Unexpected output: " + line);
        return {line.substr(0, p), s.local_endpoint().port()};
      }

      Result
      udp(std::string const& host, int port)
      {
        elle::reactor::network::UDPSocket s;
        s.close();
        s.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0));
        auto server_ep = elle::reactor::network::resolve_udp(host, port);
        s.send_to(std::string("foo\n"), server_ep[0]);
        boost::asio::ip::udp::endpoint ep;
        char buffer[5000];
        auto sz = s.receive_from(elle::WeakBuffer(buffer, sizeof buffer),
                                 ep, 5_sec);
        auto const line = std::string(buffer, sz);
        auto p = line.find(' ');
        if (line.substr(p+1) == "foo\n")
          return {line.substr(0, p), s.local_endpoint().port()};
        else
          throw std::runtime_error("Unexpected output: " + line);
      }

      std::string
      nat(std::string const& host, int port)
      {
        // check cone-nat by comparing our endpoints from rdv server and
        // connectivity server
        elle::reactor::network::RDVSocket socket;
        socket.close();
        socket.bind(
          boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port));
        // we must be polling for rdv_connect to work
        std::string result;
        bool success = true;
        elle::reactor::Thread poller(
          "poll", [&]
          {
            while (true)
            {
              elle::Buffer buf;
              buf.size(5000);
              boost::asio::ip::udp::endpoint ep;
              int sz = socket.receive_from(buf, ep);
              buf.size(sz);
              std::string line(buf.string());
              auto p = line.find(' ');
              if (line.substr(p+1) == "foo\n")
              {
                bool cone
                  = line.substr(0, p) == elle::sprintf("%s",
                                                       socket.public_endpoint());
                result = "UDP-RDV OK: ";
                if (cone)
                  result += "CONE NAT";
                else
                  result += elle::sprintf(
                    "NOT_CONE NAT %s %s", line.substr(0, p),
                    socket.public_endpoint());
              }
              else
              {
                success = false;
                result = "Unexpected output: " + line;
              }
              break;
            }
          });
        elle::SafeFinally spoll([&] {
            poller.terminate_now();
          });
        socket.rdv_connect(
          "connectivity-client-udp",
          elle::reactor::network::resolve_udp("rdv.infinit.sh", "7890")[0],
          5_sec);
        auto ep = elle::reactor::network::resolve_udp(host, port);
        socket.send_to(std::string("foo\n"), ep[0]);
        elle::reactor::wait(poller);
        spoll.abort();
        if (success)
          return result;
        else
          throw std::runtime_error(result);
      }

      Result
      rdv_utp(std::string const& host, int port, int xorit)
      {
        elle::reactor::network::UTPServer server;
        server.listen(0);
        server.xorify(xorit);
        server.rdv_connect("connectivity", "rdv.infinit.sh:7890", 5_sec);
        elle::reactor::network::UTPSocket s(server);
        s.connect("connectivity-server" + std::to_string(port), {}, 5_sec);
        s << "foo" << std::endl;
        auto const reply = s.read_until("\n", 5_sec);
        auto const line = reply.string();
        auto p = line.find(' ');
        if (line.substr(p+1) != "foo\n")
          throw std::runtime_error("Unexpected output: " + line);
        return {line.substr(0, p), server.local_endpoint().port()};
      }

      Result
      utp(std::string const& host, int port, int xorit)
      {
        elle::reactor::network::UTPServer server;
        server.listen(0);
        elle::reactor::network::UTPSocket s(server);
        server.xorify(xorit);
        s.connect(host, port);
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
}
