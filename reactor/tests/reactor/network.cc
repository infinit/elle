#include "reactor.hh"

#include <reactor/asio.hh>
#include <reactor/Scope.hh>
#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/resolve.hh>
#include <reactor/network/tcp-server.hh>
#include <reactor/network/socket.hh>
#include <reactor/signal.hh>
#include <reactor/thread.hh>

#include <elle/utility/Move.hh>
#include <elle/log.hh>
#include <elle/memory.hh>
#include <elle/test.hh>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <memory>

ELLE_LOG_COMPONENT("reactor.network.test");

using reactor::network::Byte;
using reactor::network::Buffer;
using reactor::Signal;
using reactor::network::Size;
using reactor::network::TCPSocket;
using reactor::network::TCPServer;
using reactor::Thread;

reactor::Scheduler* sched = 0;

Fixture::Fixture()
{
  sched = new reactor::Scheduler;
}

Fixture::~Fixture()
{
  delete sched;
  sched = 0;
}

template <typename Server, typename Socket>
void
silent_server()
{
  Server server(*reactor::Scheduler::scheduler());
  server.listen(4242);
  std::unique_ptr<reactor::network::Socket> socket(server.accept());
  while (true)
  {
    char buffer[512];
    try
    {
      socket->read_some(reactor::network::Buffer(buffer, sizeof(buffer)));
    }
    catch (reactor::network::ConnectionClosed const&)
    {
      break;
    }
  }
}

/*---------------.
| Destroy socket |
`---------------*/

// Find a free port. Of course there's a race condition, but it's better than an
// hardcoded one.
static
int
free_port()
{
  TCPServer s(*reactor::Scheduler::scheduler());
  s.listen();
  return s.port();
}

static
void
test_destroy_socket_non_connected()
{
  auto& sched = *reactor::Scheduler::scheduler();
  // Used to fail because shuting down a socket whose connection
  // failed used to raise.
  try
  {
    TCPSocket socket(sched, "127.0.0.1", free_port());
  }
  catch (...)
  {
    BOOST_CHECK(true);
    return;
  }
  BOOST_FAIL("Socket shouldn't have connected");
}

static
void
test_destroy_socket()
{
  reactor::Scheduler sched;
  Thread t(sched, "thread", &test_destroy_socket_non_connected);
  sched.run();
}

/*---------.
| Timeouts |
`---------*/

template <typename Server, typename Socket>
void
slowpoke_server()
{
  Server server(*sched);
  server.listen(4242);
  std::unique_ptr<reactor::network::Socket> socket(server.accept());
  sched->current()->sleep(boost::posix_time::seconds(2));
  socket->write(elle::ConstWeakBuffer("0"));
}

template <typename Server, typename Socket>
void
timeout_read()
{
  Socket socket(*sched, "127.0.0.1", 4242);
  // Poke the server to establish the pseudo connection in the UDP case.
  socket.write(elle::ConstWeakBuffer("poke"));
  Byte b;
  reactor::network::Buffer buffer(&b, 1);
  BOOST_CHECK_THROW(
    socket.read_some(buffer, boost::posix_time::milliseconds(200)),
    reactor::network::TimeOut);
  try
  {
    socket.read_some(buffer, boost::posix_time::seconds(4));
  }
  catch (reactor::network::TimeOut&)
  {
    BOOST_FAIL("read shouldn't have timed out.");
  }
  BOOST_CHECK(b == '0');
}

template <typename Server, typename Socket>
void
test_timeout_read()
{
  Fixture f;

  reactor::Thread server(*sched, "server", &slowpoke_server<Server, Socket>);
  reactor::Thread client(*sched, "read", &timeout_read<Server, Socket>);
  sched->run();
}

/*------------.
| Echo Server |
`------------*/

template <typename Server, typename Socket>
void
server();

void
serve(reactor::network::Socket* socket);

template <typename Server, typename Socket>
void
client(std::vector<std::string> messages);

template <typename Server, typename Socket>
void
server()
{
  Server server(*sched);
  server.listen(4242);
  int nclients = 2;
  std::vector<reactor::Thread*> clients;
  while (nclients--)
  {
    reactor::network::Socket* socket = server.accept();
    clients.push_back(new reactor::Thread(*sched, "serve",
                                          boost::bind(serve, socket)));
  }
  sched->current()->wait(reactor::Waitables(begin(clients), end(clients)));
  BOOST_FOREACH(auto* thread, clients)
    delete thread;
}

void
serve(reactor::network::Socket* socket)
{
  std::string received;
  Byte buffer[512];
  while (true)
  {
    Size read = 0;
    try
    {
      read = socket->read_some(Buffer(buffer, sizeof(buffer) - 1),
                               boost::posix_time::milliseconds(100));
    }
    catch (reactor::network::ConnectionClosed&)
    {
      delete socket;
      return;
    }
    catch (reactor::network::TimeOut&)
    {
      delete socket;
      return;
    }
    buffer[read] = 0;
    received += reinterpret_cast<char*>(buffer);
    size_t pos;
    while ((pos = received.find('\n')) != std::string::npos)
    {
      socket->write(elle::ConstWeakBuffer(received.c_str(), pos + 1));
      received = received.substr(pos + 1);
    }
  }
}

template <typename Server, typename Socket>
void
client(std::vector<std::string> messages, unsigned& check)
{
  Socket s(*sched, "127.0.0.1", 4242);
  BOOST_FOREACH (const std::string& message, messages)
  {
    s.write(elle::ConstWeakBuffer(message));

    Byte buf[256];
    Size read = s.read_some(Buffer(buf, 256));
    buf[read] = 0;
    BOOST_CHECK_EQUAL(message, reinterpret_cast<char*>(buf));
    ++check;
  }
}

template <typename Server, typename Socket>
void
test_echo_server()
{
  sched = new reactor::Scheduler;
  reactor::Thread s(*sched, "server", server<Server, Socket>);

  unsigned check_1 = 0;
  std::vector<std::string> messages_1;
  messages_1.push_back("Hello server!\n");
  messages_1.push_back("How are you?\n");
  reactor::Thread c1(*sched, "client1",
                     boost::bind(client<Server, Socket>,
                                 messages_1, boost::ref(check_1)));

  unsigned check_2 = 0;
  std::vector<std::string> messages_2;
  messages_2.push_back("Lorem ipsum dolor sit amet, "
                     "consectetur adipiscing elit.\n");
  messages_2.push_back("Phasellus gravida auctor felis, "
                     "sed eleifend turpis commodo pretium.\n");
  messages_2.push_back("Vestibulum ante ipsum primis in faucibus orci "
                     "luctus et ultrices posuere cubilia Curae; "
                     "Proin porttitor cursus ornare.\n");
  messages_2.push_back("Praesent sodales sodales est non placerat.\n");
  messages_2.push_back("Etiam iaculis ultrices libero ac ultrices.\n");
  messages_2.push_back("Integer ultricies pharetra tempus.\n");
  messages_2.push_back("Morbi metus ligula, facilisis tristique interdum et,"
                     "tincidunt eget tellus.\n");
  messages_2.push_back("Sed a lacinia turpis.\n");
  messages_2.push_back("Vestibulum leo tellus, ultrices a convallis eget, "
                     "cursus id dolor.\n");
  messages_2.push_back("Lorem ipsum dolor sit amet, "
                     "consectetur adipiscing elit.\n");
  messages_2.push_back("Aliquam erat volutpat.\n");
  reactor::Thread c2(*sched, "client2",
                     boost::bind(client<Server, Socket>,
                                 messages_2, boost::ref(check_2)));

  sched->run();
  BOOST_CHECK_EQUAL(check_1, messages_1.size());
  BOOST_CHECK_EQUAL(check_2, messages_2.size());
  delete sched;
}

/*-------------------.
| Socket destruction |
`-------------------*/

static
void
test_socket_destruction()
{
  reactor::Scheduler sched;

  reactor::Thread server(sched, "server", &silent_server<TCPServer, TCPSocket>);


  auto action = [&] ()
    {
      auto socket = elle::make_unique<reactor::network::TCPSocket>(
        sched, "127.0.0.1", 4242);
      *socket << "foo";
      socket->socket()->close();
      // Check the IOStream doesn't try to flush the buffer if the TCPSocket
      // failed at it.
      // XXX: Sort this out when socket destruction is handled.
      /*BOOST_CHECK_THROW(*/delete socket.release()/*, elle::Exception)*/;
    };

  reactor::Thread t(sched, "client", action);

  sched.run();
}


/*-------------.
| Socket close |
`-------------*/

static
void
test_socket_close()
{
  reactor::Scheduler sched;

  reactor::Thread server(sched, "server", &silent_server<TCPServer, TCPSocket>);

  auto action = [&] ()
    {
      auto socket = elle::make_unique<reactor::network::TCPSocket>(
        sched, "127.0.0.1", 4242);
      elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
      {
        scope.run_background(
          "read",
          [&]
          {
            BOOST_CHECK_THROW(socket->get(), elle::Exception);
          });
        scope.run_background(
          "close",
          [&]
          {
            reactor::yield();
            socket->close();
          });
        scope.wait();
      };
    };

  reactor::Thread t(sched, "client", action);

  sched.run();
}

/*-------------------.
| Resolution failure |
`-------------------*/

static
void
resolution_failure()
{
  reactor::Scheduler sched;
  reactor::Thread t(
    sched, "resolver",
    [&]
    {
      BOOST_CHECK_THROW(
        reactor::network::resolve_tcp(sched, "does.not.exist", "http"),
        reactor::network::ResolutionError);
    });
  sched.run();
}

/*-----------.
| Read until |
`-----------*/


class Server
{
public:
  Server():
    _server(),
    _port(0)
  {
    this->_server.listen(0);
    this->_port = this->_server.port();
    ELLE_LOG("%s: listen on port %s", *this, this->_port);
    this->_accepter.reset(
      new reactor::Thread(*reactor::Scheduler::scheduler(),
                          "accepter",
                          std::bind(&Server::_accept,
                                    std::ref(*this))));
  }

  ~Server()
  {
    this->_accepter->terminate_now();
  }

  ELLE_ATTRIBUTE(reactor::network::TCPServer, server);
  ELLE_ATTRIBUTE_R(int, port);
  ELLE_ATTRIBUTE(std::unique_ptr<reactor::Thread>, accepter);

private:
  void
  _accept()
  {
    elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
    {
      while (true)
      {
        std::unique_ptr<reactor::network::TCPSocket> socket(
          this->_server.accept());
        ELLE_LOG("accept connection from %s", socket->peer());
        auto name = elle::sprintf("request %s", socket->peer());
        scope.run_background(
          name,
          std::bind(&Server::_serve, std::ref(*this),
                    elle::utility::move_on_copy(socket)));
      }
    };
  }

  virtual
  void
  _serve(std::unique_ptr<reactor::network::TCPSocket> socket) = 0;
};

class ContentServer:
  public Server
{
public:
  ContentServer(std::string const& content):
    Server(),
    _content(content)
  {}

  virtual
  void
  _serve(std::unique_ptr<reactor::network::TCPSocket> socket) override
  {
    socket->write(this->_content);
  }

  ELLE_ATTRIBUTE_R(std::string, content);
};

static
void
read_until()
{
  reactor::Scheduler sched;

  reactor::Thread main(
    sched, "main",
    []
    {
      ContentServer server("foo\r\nx\r\nbar\r\n");

      reactor::network::TCPSocket sock("127.0.0.1", server.port());
      BOOST_CHECK_EQUAL(sock.read_until("\r\n"), "foo\r\n");
      char c;
      sock.read(reactor::network::Buffer(&c, 1));
      BOOST_CHECK_EQUAL(c, 'x');
      BOOST_CHECK_EQUAL(sock.read_until("\r\n"), "\r\n");
      BOOST_CHECK_EQUAL(sock.read_until("\r\n"), "bar\r\n");
    });

  sched.run();
}


/*-----------.
| Test suite |
`-----------*/

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(test_destroy_socket));
#define INFINIT_REACTOR_NETWORK_TEST(Proto)                             \
  suite.add(BOOST_TEST_CASE((test_timeout_read                          \
                                <Proto##Server, Proto##Socket>)));      \
  suite.add(BOOST_TEST_CASE((test_echo_server                           \
                                <Proto##Server, Proto##Socket>)));      \

  INFINIT_REACTOR_NETWORK_TEST(TCP);
#undef INFINIT_REACTOR_NETWORK_TEST
  suite.add(BOOST_TEST_CASE(test_socket_destruction));
  suite.add(BOOST_TEST_CASE(test_socket_close));
  suite.add(BOOST_TEST_CASE(resolution_failure));
  suite.add(BOOST_TEST_CASE(read_until));
}
