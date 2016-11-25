#include <memory>

#include <boost/bind.hpp>

#include <elle/log.hh>
#include <elle/memory.hh>
#include <elle/test.hh>
#include <elle/utility/Move.hh>

#include <reactor/asio.hh>
#include <reactor/Scope.hh>
#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/resolve.hh>
#include <reactor/network/tcp-server.hh>
#include <reactor/network/tcp-socket.hh>
#include <reactor/network/server.hh>
#include <reactor/network/socket.hh>
#ifdef REACTOR_NETWORK_UNIX_DOMAIN_SOCKET
# include <reactor/network/unix-domain-server.hh>
# include <reactor/network/unix-domain-socket.hh>
#endif
#include <reactor/signal.hh>
#include <reactor/thread.hh>

#include "reactor.hh"

ELLE_LOG_COMPONENT("reactor.network.test");

using reactor::network::Byte;
using reactor::network::Buffer;
using reactor::Signal;
using reactor::network::TCPSocket;
using reactor::network::TCPServer;
#ifdef REACTOR_NETWORK_UNIX_DOMAIN_SOCKET
using reactor::network::UnixDomainSocket;
using reactor::network::UnixDomainServer;
#endif
using reactor::Thread;

template <typename Server, typename Socket>
class SilentServer
  : public Server
{
public:
  template <typename ... Args>
  SilentServer(Args&& ... args)
    : Server(std::forward<Args>(args)...)
  {
    this->listen();
    this->_server_thread.reset(
      new reactor::Thread(elle::sprintf("%s: serve", this),
                          [this] { this->silent_server(); }));
  }

  void
  silent_server()
  {
    std::unique_ptr<reactor::network::Socket> socket(this->accept());
    while (true)
    {
      char buffer[512];
      try
      {
        socket->read_some(reactor::network::Buffer(buffer, sizeof buffer));
      }
      catch (reactor::network::ConnectionClosed const&)
      {
        break;
      }
    }
  }

  ELLE_ATTRIBUTE(reactor::Thread::unique_ptr, server_thread);
};


/*---------------.
| Destroy socket |
`---------------*/

// Connection refused entails infinite wait on wine.
#ifndef INFINIT_WINDOWS
// Find a free port. Of course there's a race condition, but it's better than an
// hardcoded one.
static
int
free_port()
{
  TCPServer s{};
  s.listen();
  return s.port();
}

static
void
test_destroy_socket_non_connected()
{
  // Used to fail because shuting down a socket whose connection
  // failed used to raise.
  try
  {
    TCPSocket socket("127.0.0.1", free_port());
  }
  catch (...)
  {
    BOOST_CHECK(true);
    return;
  }
  BOOST_FAIL("Socket shouldn't have connected. "
             "Boost 1.54 has this bug, check your install.");
}

static
void
destroy_socket()
{
  reactor::Scheduler sched;
  Thread t(sched, "thread", &test_destroy_socket_non_connected);
  sched.run();
}
#endif

/*---------.
| Timeouts |
`---------*/

template <typename Server, typename Socket>
void
test_timeout_read()
{
  reactor::Scheduler sched;
  reactor::Thread main(
    sched,
    "main",
    []
    {
      Server server;
      server.listen();
      reactor::Thread server_thread(
        "server",
        [&]
        {
          std::unique_ptr<reactor::network::Socket> socket(server.accept());
          reactor::sleep(2_sec);
          socket->write(elle::ConstWeakBuffer("0"));
        });
      Socket socket(server.local_endpoint());
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
      reactor::wait(server_thread);
     });
  sched.run();
}

/*------------.
| Echo Server |
`------------*/

template <typename Socket>
void
serve(std::unique_ptr<Socket> socket)
{
  std::string received;
  Byte buffer[512];
  while (true)
  {
    reactor::network::Size read = 0;
    try
    {
      read = socket->read_some(Buffer(buffer, sizeof buffer - 1),
                               boost::posix_time::milliseconds(100));
    }
    catch (reactor::network::ConnectionClosed&)
    {
      break;
    }
    catch (reactor::network::TimeOut&)
    {
      break;
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
client(typename Socket::EndPoint const& ep,
       std::vector<std::string> messages, unsigned& check)
{
  Socket s(ep);
  for (const std::string& message: messages)
  {
    Byte buf[256];
    assert(message.size() + 1 < sizeof buf);
    s.write(elle::ConstWeakBuffer(message));
    reactor::network::Size read = s.read_some(Buffer(buf, sizeof buf));
    buf[read] = 0;
    BOOST_CHECK_EQUAL(message, reinterpret_cast<char*>(buf));
    ++check;
  }
}

template <typename Server, typename Socket>
void
test_echo_server()
{
  reactor::Scheduler sched;
  unsigned check_1 = 0;
  auto messages_1 = std::vector<std::string>
    {
      "Hello server!\n",
      "How are you?\n",
    };
  unsigned check_2 = 0;
  auto messages_2 = std::vector<std::string>
    {
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n",
      "Phasellus gravida auctor felis, "
      "sed eleifend turpis commodo pretium.\n",
      "Vestibulum ante ipsum primis in faucibus orci "
      "luctus et ultrices posuere cubilia Curae; "
      "Proin porttitor cursus ornare.\n",
      "Praesent sodales sodales est non placerat.\n",
      "Etiam iaculis ultrices libero ac ultrices.\n",
      "Integer ultricies pharetra tempus.\n",
      "Morbi metus ligula, facilisis tristique interdum et,"
      "tincidunt eget tellus.\n",
      "Sed a lacinia turpis.\n",
      "Vestibulum leo tellus, ultrices a convallis eget, cursus id dolor.\n",
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n",
      "Aliquam erat volutpat.\n",
    };
  reactor::Thread main(
    sched,
    "main",
    [&]
    {
      Server server;
      server.listen();
      reactor::Thread s(
        "server",
        [&]
        {
          int nclients = 2;
          std::vector<reactor::Thread*> clients;
          while (nclients--)
          {
            auto socket = server.accept();
            clients.push_back(
              new reactor::Thread(
                "serve",
                boost::bind(&serve<Socket>,
                            elle::utility::move_on_copy(socket))));
          }
          reactor::wait(reactor::Waitables(begin(clients), end(clients)));
          for (auto* thread: clients)
            delete thread;
        });
      reactor::Thread c1("client1",
                         boost::bind(client<Server, Socket>,
                                     server.local_endpoint(),
                                     messages_1, std::ref(check_1)));
      reactor::Thread c2("client2",
                         boost::bind(client<Server, Socket>,
                                     server.local_endpoint(),
                                     messages_2, std::ref(check_2)));
      reactor::wait(c1);
      reactor::wait(c2);
      reactor::wait(s);
    });
  sched.run();
  BOOST_CHECK_EQUAL(check_1, messages_1.size());
  BOOST_CHECK_EQUAL(check_2, messages_2.size());
}

/*-------------------.
| Socket destruction |
`-------------------*/

ELLE_TEST_SCHEDULED(socket_destruction)
{
  SilentServer<TCPServer, TCPSocket> server;
  auto socket =
    elle::make_unique<reactor::network::TCPSocket>(server.local_endpoint());
  *socket << "foo";
  socket->socket()->close();
  // Check the IOStream doesn't try to flush the buffer if the TCPSocket
  // failed at it.
  // XXX: Sort this out when socket destruction is handled.
  /*BOOST_CHECK_THROW(*/delete socket.release()/*, elle::Exception)*/;
}

/*-------------------.
| Connection refused |
`-------------------*/

ELLE_TEST_SCHEDULED(connection_refused)
{
  BOOST_CHECK_THROW(
    elle::make_unique<reactor::network::TCPSocket>("localhost", 0),
    reactor::network::ConnectionRefused);
}


/*-------------.
| Socket close |
`-------------*/

static
void
socket_close()
{
  reactor::Scheduler sched;
  auto main =
    [&]
    {
      SilentServer<TCPServer, TCPSocket> server;
      auto socket =
        elle::make_unique<reactor::network::TCPSocket>(server.local_endpoint());
      elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
      {
        scope.run_background(
          "read",
          [&]
          {
            BOOST_CHECK_THROW(socket->get(),
                              reactor::network::ConnectionClosed);
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
  reactor::Thread t(sched, "client", main);
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
        reactor::network::resolve_tcp("does.not.exist", "http"),
        reactor::network::ResolutionError);
      BOOST_CHECK_THROW(
        reactor::network::TCPSocket("does.not.exist", "http"),
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
        std::unique_ptr<reactor::network::Socket> socket(
          this->_server.accept());
        ELLE_LOG("accept connection: %s", socket);
        auto name = elle::sprintf("request %s", socket);
        scope.run_background(
          name,
          std::bind(&Server::_serve, std::ref(*this),
                    elle::utility::move_on_copy(socket)));
      }
    };
  }

  virtual
  void
  _serve(std::unique_ptr<reactor::network::Socket> socket) = 0;
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
  _serve(std::unique_ptr<reactor::network::Socket> socket) override
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

/*----------.
| underflow |
`----------*/

// Check that when a read is complete, the next buffer isn't loaded though
// std::streambuf::underflow, entailing dead locks. This happens on OSX lion
// (10.7).
static
void
underflow()
{
  reactor::Scheduler sched;
  reactor::Barrier listening;
  int port = 0;
  reactor::Thread t(
    sched, "main",
    [&]
    {
      elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
      {
        scope.run_background(
          "server",
          [&]
          {
            reactor::network::TCPServer server;
            server.listen(0);
            port = server.port();
            listening.open();
            std::unique_ptr<reactor::network::Socket> socket(
              server.accept());
            socket->write(std::string("lulz"));
            socket->write(std::string("lol"));
          });
        scope.run_background(
          "client",
          [&]
          {
            reactor::wait(listening);
            reactor::network::TCPSocket socket("127.0.0.1", port);
            socket.write(std::string("lulz"));
            char data[1024];
            ELLE_LOG("first read")
              socket.std::iostream::read(data, 4);
            ELLE_LOG("second read")
              socket.std::iostream::read(data, 3);
          });
        reactor::wait(scope);
      };
    });
  sched.run();
}

/*------------------------.
| Read / write cancelling |
`------------------------*/

ELLE_TEST_SCHEDULED(read_write_cancel)
{
  reactor::Barrier listening;
  int port;
  elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
  {
    auto& server = scope.run_background(
      "server",
      [&]
      {
        reactor::network::TCPServer server;
        server.listen();
        port = server.port();
        listening.open();
        auto client = server.accept();
        reactor::sleep();
      });
    scope.run_background(
      "client",
      [&]
      {
        reactor::wait(listening);
        reactor::network::TCPSocket socket("127.0.0.1", port);
        elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
        {
          scope.run_background(
            "reader",
            [&]
            {
              BOOST_CHECK_THROW(socket.read(1, 1_sec),
                                reactor::network::TimeOut);
            });
          auto& writer = scope.run_background(
            "writer",
            [&]
            {
              socket.write(elle::ConstWeakBuffer("1"));
            });
          scope.run_background(
            "",
            [&]
            {
              // Check that terminating the write, that is done but has not yet
              // given back control to its fiber, does not cancel the read
              // operation on the socket.
              writer.terminate();
            });
          reactor::wait(scope);
          server.terminate();
        };
      });
    reactor::wait(scope);
  };
}

/*-----------------.
| Resolution abort |
`-----------------*/

ELLE_TEST_SCHEDULED(resolution_abort)
{
  elle::With<reactor::Scope>() << [] (reactor::Scope& scope)
  {
    auto& resolve = scope.run_background(
      "resolve",
      []
      {
        reactor::network::resolve_tcp("bertier.lacogip.fr", "80");
      });
    scope.run_background(
      "kill",
      [&]
      {
        resolve.terminate_now();
      });
    reactor::wait(scope);
  };
}

ELLE_TEST_SCHEDULED(read_terminate_recover)
{
  char wbuf[100];
  for (auto i = 0u; i < sizeof wbuf; ++i)
    wbuf[i] = rand();
  reactor::network::TCPServer server;
  server.listen();
  reactor::Barrier terminated;
  reactor::Barrier reading;
  reactor::Barrier written;
  reactor::Thread accept(
    "accept",
    [&]
    {
      auto socket = server.accept();
      char buffer[100] = {char(0xfd)};
      int bytes_read = 0;
      try
      {
        // Wait until the data is written to ensure we have a chance to read it
        // and not be killed right away.
        reactor::wait(written);
        reading.open();
        socket->read(reactor::network::Buffer(buffer, sizeof buffer),
                     elle::DurationOpt(), &bytes_read);
      }
      catch (reactor::Terminate const& e)
      {
        terminated.open();
        BOOST_CHECK_EQUAL(bytes_read, 50);
        BOOST_CHECK(!memcmp(buffer, wbuf, 50));
        socket->read(reactor::network::Buffer(buffer + bytes_read,
                                              sizeof buffer - bytes_read));
        BOOST_CHECK(!memcmp(buffer, wbuf, sizeof wbuf));
        throw;
      }
      BOOST_FAIL("unreachable");
    });
  reactor::network::TCPSocket socket(
    "localhost", server.local_endpoint().port());
  ELLE_LOG("write 50")
    socket.write(elle::ConstWeakBuffer(wbuf, 50));
  written.open();
  reactor::wait(reading);
  ELLE_LOG("kill reader")
    accept.terminate();
  reactor::wait(terminated);
  ELLE_LOG("write 50")
    socket.write(elle::ConstWeakBuffer(wbuf+50, 50));
  reactor::wait(accept);
}

ELLE_TEST_SCHEDULED(read_terminate_recover_iostream)
{
  char wbuf[100];
  for (auto i = 0u; i < sizeof wbuf; ++i)
    wbuf[i] = rand();
  reactor::network::TCPServer server;
  server.listen();
  reactor::Barrier terminated;
  reactor::Barrier reading;
  reactor::Thread accept(
    "accept",
    [&]
    {
      auto socket = server.accept();
      char buffer[101] = {char(0xfd)};
      int read = 0;
      try
      {
        elle::IOStreamClear clearer(*socket);
        reading.open();
        ELLE_LOG("read 100 bytes")
          while (read < 100)
            read +=
              std::readsome(*socket, buffer + read, sizeof buffer - read);
      }
      catch (reactor::Terminate const& e)
      {
        terminated.open();
        while (read < 100)
          read +=
            std::readsome(*socket, buffer + read, sizeof buffer - read);
        BOOST_CHECK(!memcmp(buffer, wbuf, sizeof wbuf));
        throw;
      }
      BOOST_FAIL("unreachable");
    });
  reactor::network::TCPSocket socket(
    "localhost", server.local_endpoint().port());
  ELLE_LOG("write 50")
    socket.write(elle::ConstWeakBuffer(wbuf, 50));
  reactor::wait(reading);
  ELLE_LOG("kill reader")
    accept.terminate();
  reactor::wait(terminated);
  ELLE_LOG("write 50")
    socket.write(elle::ConstWeakBuffer(wbuf + 50, 50));
  socket.close();
  reactor::wait(accept);
}

ELLE_TEST_SCHEDULED(read_terminate_deadlock)
{
  reactor::network::TCPServer server;
  server.listen();
  reactor::Barrier terminated;
  reactor::Barrier reading;
  reactor::Thread accept(
    "accept",
    [&]
    {
      auto socket = server.accept();
      char buffer[8] = {static_cast<char>(0xfd)};
      int read = 0;
      elle::IOStreamClear clearer(*socket);
      reading.open();
      while (read < 8)
        read += std::readsome(*socket, buffer + read, sizeof(buffer) - read);
      reactor::sleep();
    });
  reactor::network::TCPSocket socket(
    "localhost", server.local_endpoint().port());
  reactor::wait(reading);
  ELLE_LOG("write 8")
    socket.write(elle::ConstWeakBuffer("abcdefgh", 8));
  accept.terminate_now();
}

/*-----------.
| Test suite |
`-----------*/

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
#ifndef INFINIT_WINDOWS
  suite.add(BOOST_TEST_CASE(destroy_socket), 0, 10);
#endif
#define INFINIT_REACTOR_NETWORK_TEST(Proto)                             \
  {                                                                     \
    auto subsuite = BOOST_TEST_SUITE(#Proto);                           \
    suite.add(subsuite);                                                \
    auto timeout_read =                                                 \
      &test_timeout_read<Proto##Server, Proto##Socket>;                 \
    subsuite->add(BOOST_TEST_CASE(timeout_read), 0, 10);                \
    auto echo_server =                                                  \
      &test_echo_server<Proto##Server, Proto##Socket>;                  \
    subsuite->add(BOOST_TEST_CASE(echo_server), 0, 10);                 \
  }                                                                     \

  INFINIT_REACTOR_NETWORK_TEST(TCP);
#ifdef REACTOR_NETWORK_UNIX_DOMAIN_SOCKET
  INFINIT_REACTOR_NETWORK_TEST(UnixDomain);
#endif
#undef INFINIT_REACTOR_NETWORK_TEST
  suite.add(BOOST_TEST_CASE(socket_destruction), 0, 10);
  suite.add(BOOST_TEST_CASE(connection_refused), 0, 1);
  suite.add(BOOST_TEST_CASE(socket_close), 0, 10);
  suite.add(BOOST_TEST_CASE(resolution_failure), 0, 10);
  suite.add(BOOST_TEST_CASE(read_until), 0, 10);
  suite.add(BOOST_TEST_CASE(underflow), 0, 10);
  suite.add(BOOST_TEST_CASE(read_write_cancel), 0, 10);
  suite.add(BOOST_TEST_CASE(resolution_abort), 0, 2);
  suite.add(BOOST_TEST_CASE(read_terminate_recover), 0, 1);
  suite.add(BOOST_TEST_CASE(read_terminate_recover_iostream), 0, 1);
  suite.add(BOOST_TEST_CASE(read_terminate_deadlock), 0, 1);
}
