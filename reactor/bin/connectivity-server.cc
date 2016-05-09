#include <elle/utility/Move.hh>

#include <reactor/network/rdv.hh>

#include <reactor/network/buffer.hh>
#include <reactor/network/udp-socket.hh>
#include <reactor/network/udp-server.hh>
#include <reactor/network/tcp-socket.hh>
#include <reactor/network/tcp-server.hh>
#include <reactor/network/utp-socket.hh>
#include <reactor/scheduler.hh>

ELLE_LOG_COMPONENT("connectivity-server");

static
void
serve_tcp(int port)
{
  auto server = elle::make_unique<reactor::network::TCPServer>();
  server->listen(port);
  while (true)
  {
    auto socket = elle::utility::move_on_copy(server->accept());
    new reactor::Thread(
      "serve",
      [socket]
      {
        try
        {
          ELLE_TRACE("serving %s", socket);
          while (true)
          {
            std::string line;
            std::getline(**socket, line);
            **socket << elle::sprintf("%s", (*socket)->peer()) << ' ' << line << std::endl;
          }
        }
        catch (elle::Error const& e)
        {
          ELLE_DEBUG("lost TCP %s: %s", (*socket)->peer(), e);
        }
    }, true);
  }
}

static
void
serve_udp(int port)
{
  auto server = elle::make_unique<reactor::network::UDPSocket>();
  server->close();
  server->bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port));
  while (true)
  {
    elle::Buffer buf;
    buf.size(5000);
    boost::asio::ip::udp::endpoint ep;
    int sz = server->receive_from(reactor::network::Buffer(buf.mutable_contents(),
                                                           buf.size()), ep);
    buf.size(sz);
    ELLE_TRACE("received UDP %s", ep);
    auto reply = elle::sprintf("%s %s", ep, buf.string());
    server->send_to(reactor::network::Buffer(reply.data(), reply.size()), ep);
  }
}

// raw udp with rdv
static void serve_rdv(int port)
{
  reactor::network::RDVSocket socket;
  socket.close();
  socket.bind(
    boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port));
  new reactor::Thread("rdv_connect", [&] {
      socket.rdv_connect("connectivity-server-udp", "rdv.infinit.sh", 7890);
  }, true);
  while (true)
  {
    elle::Buffer buf;
    buf.size(5000);
    boost::asio::ip::udp::endpoint ep;
    int sz = socket.receive_from(reactor::network::Buffer(buf.mutable_contents(),
                                                           buf.size()), ep);
    buf.size(sz);
    ELLE_TRACE("received UDP %s", ep);
    auto reply = elle::sprintf("%s %s", ep, buf.string());
    socket.send_to(reactor::network::Buffer(reply.data(), reply.size()), ep);
  }
}

static void serve_utp(int port)
{
  reactor::network::UTPServer server;
  server.listen(port);
  server.rdv_connect("connectivity-server", "rdv.infinit.sh:7890");
  while (true)
  {
    auto socket = server.accept().release();
    //auto socket = elle::utility::move_on_copy(server.accept());
    new reactor::Thread("serve", [socket] {
        try
        {
          ELLE_TRACE("serving UTP %s(%s)", socket, socket->peer());
          while (true)
          {
            std::string line;
            std::getline(*socket, line);
            *socket << elle::sprintf("%s", socket->peer()) << ' ' << line << std::endl;
          }
        }
        catch (std::ios_base::failure const& e)
        {
          ELLE_DEBUG("lost UTP %s: %s", socket, e);
        }
        catch (elle::Error const& e)
        {
          ELLE_DEBUG("lost UTP %s: %s", socket, e);
        }
        delete socket;
    }, true);
  }
}

static void run(int argc, char** argv)
{
  int port = 5456;
  new reactor::Thread("tcp", [port] { serve_tcp(port);});
  new reactor::Thread("udp", [port] { serve_udp(port);});
  new reactor::Thread("rdv_utp", [port] { serve_utp(port+1);});
  new reactor::Thread("rdv_utp", [port] { serve_rdv(0);});
}

int main(int argc, char** argv)
{
  reactor::Scheduler sched;
  reactor::Thread t(sched, "main", [&]
    {
      run(argc, argv);
    });
  sched.run();
}
