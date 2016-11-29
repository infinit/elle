#include <elle/utility/Move.hh>

#include <reactor/network/rdv-socket.hh>
#include <reactor/network/rdv.hh>
#include <reactor/network/tcp-server.hh>
#include <reactor/network/tcp-socket.hh>
#include <reactor/network/udp-socket.hh>
#include <reactor/network/utp-server.hh>
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
            **socket << (*socket)->peer() << ' ' << line << std::endl;
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
    int sz = server->receive_from(elle::WeakBuffer(buf), ep);
    buf.size(sz);
    ELLE_TRACE("received UDP %s", ep);
    auto reply = elle::sprintf("%s %s", ep, buf.string());
    server->send_to(elle::ConstWeakBuffer(reply), ep);
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
    int sz = socket.receive_from(elle::WeakBuffer(buf), ep);
    buf.size(sz);
    ELLE_TRACE("received UDP %s", ep);
    auto reply = elle::sprintf("%s %s", ep, buf.string());
    socket.send_to(elle::ConstWeakBuffer(reply), ep);
  }
}

static void serve_utp(int port, int xorit)
{
  reactor::network::UTPServer server;
  server.xorify(xorit);
  server.listen(port);
  server.rdv_connect("connectivity-server" + std::to_string(port),
                     "rdv.infinit.sh:7890");
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
            *socket << socket->peer() << ' ' << line << std::endl;
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
  new reactor::Thread("rdv_utp", [port] { serve_utp(port+1, 0);});
  new reactor::Thread("rdv_utp_xor", [port] { serve_utp(port+2, 0xFF);});
  new reactor::Thread("rdv_udp", [port] { serve_rdv(0);});
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
