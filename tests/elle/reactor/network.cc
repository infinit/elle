#include <memory>
#include <utility>

#include <boost/bind.hpp>

#include <elle/Buffer.hh>
#include <elle/log.hh>
#include <elle/memory.hh>
#include <elle/os/environ.hh>
#include <elle/test.hh>
#include <elle/utility/Move.hh>

#include <elle/reactor/asio.hh>
#include <elle/reactor/Scope.hh>
#include <elle/reactor/network/Error.hh>
#include <elle/reactor/network/resolve.hh>
#include <elle/reactor/network/tcp-server.hh>
#include <elle/reactor/network/tcp-socket.hh>
#include <elle/reactor/network/server.hh>
#include <elle/reactor/network/socket.hh>
#ifdef REACTOR_NETWORK_UNIX_DOMAIN_SOCKET
# include <elle/reactor/network/unix-domain-server.hh>
# include <elle/reactor/network/unix-domain-socket.hh>
#endif
#include <elle/reactor/signal.hh>
#include <elle/reactor/Thread.hh>

#include "reactor.hh"

ELLE_LOG_COMPONENT("elle.reactor.network.test");

using elle::reactor::network::Byte;
using elle::reactor::Signal;
using elle::reactor::network::TCPSocket;
using elle::reactor::network::TCPServer;
#ifdef REACTOR_NETWORK_UNIX_DOMAIN_SOCKET
using elle::reactor::network::UnixDomainSocket;
using elle::reactor::network::UnixDomainServer;
#endif
using elle::reactor::Thread;

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
      new elle::reactor::Thread(elle::sprintf("%s: serve", this),
                                [this] { this->silent_server(); }));
  }

  void
  silent_server()
  {
    std::unique_ptr<elle::reactor::network::Socket> socket = this->accept();
    while (true)
    {
      char buffer[512];
      try
      {
        socket->read_some(elle::WeakBuffer(buffer));
      }
      catch (elle::reactor::network::ConnectionClosed const&)
      {
        break;
      }
    }
  }

  ELLE_ATTRIBUTE(elle::reactor::Thread::unique_ptr, server_thread);
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
  elle::reactor::Scheduler sched;
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
  elle::reactor::Scheduler sched;
  elle::reactor::Thread main(
    sched,
    "main",
    []
    {
      Server server;
      server.listen();
      elle::reactor::Thread server_thread(
        "server",
        [&]
        {
          std::unique_ptr<elle::reactor::network::Socket> socket(server.accept());
          elle::reactor::sleep(2_sec);
          socket->write(elle::ConstWeakBuffer("0"));
        });
      Socket socket(server.local_endpoint());
      // Poke the server to establish the pseudo connection in the UDP case.
      socket.write(elle::ConstWeakBuffer("poke"));
      Byte b;
      auto buffer = elle::WeakBuffer(&b, 1);
      BOOST_CHECK_THROW(
        socket.read_some(buffer, boost::posix_time::milliseconds(200)),
        elle::reactor::network::TimeOut);
      try
      {
        socket.read_some(buffer, boost::posix_time::seconds(4));
      }
      catch (elle::reactor::network::TimeOut&)
      {
        BOOST_FAIL("read shouldn't have timed out.");
      }
      BOOST_CHECK(b == '0');
      elle::reactor::wait(server_thread);
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
    elle::reactor::network::Size read = 0;
    try
    {
      read = socket->read_some(elle::WeakBuffer(buffer, sizeof buffer - 1),
                               boost::posix_time::milliseconds(100));
    }
    catch (elle::reactor::network::ConnectionClosed&)
    {
      break;
    }
    catch (elle::reactor::network::TimeOut&)
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
    elle::reactor::network::Size read = s.read_some(elle::WeakBuffer(buf));
    buf[read] = 0;
    BOOST_CHECK_EQUAL(message, reinterpret_cast<char*>(buf));
    ++check;
  }
}

template <typename Server, typename Socket>
void
test_echo_server()
{
  elle::reactor::Scheduler sched;
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
  elle::reactor::Thread main(
    sched,
    "main",
    [&]
    {
      Server server;
      server.listen();
      elle::reactor::Thread s(
        "server",
        [&]
        {
          int nclients = 2;
          std::vector<elle::reactor::Thread*> clients;
          while (nclients--)
          {
            auto socket = server.accept();
            clients.push_back(
              new elle::reactor::Thread(
                "serve",
                std::bind(&serve<Socket>,
                            elle::utility::move_on_copy(socket))));
          }
          elle::reactor::wait(elle::reactor::Waitables(begin(clients), end(clients)));
          for (auto* thread: clients)
            delete thread;
        });
      elle::reactor::Thread c1("client1",
                         std::bind(client<Server, Socket>,
                                     server.local_endpoint(),
                                     messages_1, std::ref(check_1)));
      elle::reactor::Thread c2("client2",
                         std::bind(client<Server, Socket>,
                                     server.local_endpoint(),
                                     messages_2, std::ref(check_2)));
      elle::reactor::wait(c1);
      elle::reactor::wait(c2);
      elle::reactor::wait(s);
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
    std::make_unique<elle::reactor::network::TCPSocket>(server.local_endpoint());
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
    std::make_unique<elle::reactor::network::TCPSocket>("localhost", 0),
    elle::reactor::network::ConnectionRefused);
}


/*-------------.
| Socket close |
`-------------*/

static
void
socket_close()
{
  elle::reactor::Scheduler sched;
  auto main =
    [&]
    {
      SilentServer<TCPServer, TCPSocket> server;
      auto socket =
        std::make_unique<elle::reactor::network::TCPSocket>(server.local_endpoint());
      elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& scope)
      {
        scope.run_background(
          "read",
          [&]
          {
            BOOST_CHECK_THROW(socket->get(),
                              elle::reactor::network::ConnectionClosed);
          });
        scope.run_background(
          "close",
          [&]
          {
            elle::reactor::yield();
            socket->close();
          });
        scope.wait();
      };
    };
  elle::reactor::Thread t(sched, "client", main);
  sched.run();
}

/*-------------------.
| Resolution failure |
`-------------------*/

namespace
{
void
resolution_failure()
{
  elle::reactor::Scheduler sched;
  elle::reactor::Thread t(
    sched, "resolver",
    [&]
    {
      elle::os::unsetenv("ELLE_REACTOR_RESOLVE_TRY_AGAIN");
      BOOST_CHECK_THROW(
        elle::reactor::network::resolve_tcp("does.not.exist", "http"),
        elle::reactor::network::ResolutionError);
      BOOST_CHECK_THROW(
        elle::reactor::network::resolve_tcp("does.not.exist", 80),
        elle::reactor::network::ResolutionError);
      BOOST_CHECK_THROW(
        elle::reactor::network::resolve_tcp_repr("does.not.exist:http"),
        elle::reactor::network::ResolutionError);
      BOOST_CHECK_THROW(
        elle::reactor::network::resolve_tcp_repr("does.not.exist:80"),
        elle::reactor::network::ResolutionError);
      BOOST_CHECK_THROW(
        elle::reactor::network::TCPSocket("does.not.exist", "http"),
        elle::reactor::network::ResolutionError);

      elle::os::setenv("ELLE_REACTOR_RESOLVE_TRY_AGAIN", "1", true);
      BOOST_CHECK_THROW(
        elle::reactor::network::resolve_tcp_repr("localhost:80"),
        elle::reactor::network::ResolutionError);
      elle::os::unsetenv("ELLE_REACTOR_RESOLVE_TRY_AGAIN");
    });
  sched.run();
}
}

/*-----------.
| Read until |
`-----------*/

class Server
{
public:
  Server():
    _server()
  {
    this->_server.listen(0);
    this->_port = this->_server.port();
    ELLE_LOG("%s: listen on port %s", *this, this->_port);
    this->_accepter.reset(
      new elle::reactor::Thread(*elle::reactor::Scheduler::scheduler(),
                          "accepter",
                          std::bind(&Server::_accept,
                                    std::ref(*this))));
  }

  ~Server()
  {
    this->_accepter->terminate_now();
  }

  ELLE_ATTRIBUTE(elle::reactor::network::TCPServer, server);
  ELLE_ATTRIBUTE_R(int, port);
  ELLE_ATTRIBUTE(std::unique_ptr<elle::reactor::Thread>, accepter);

private:
  void
  _accept()
  {
    elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& scope)
    {
      while (true)
      {
        std::unique_ptr<elle::reactor::network::Socket> socket(
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
  _serve(std::unique_ptr<elle::reactor::network::Socket> socket) = 0;
};

class ContentServer:
  public Server
{
public:
  ContentServer(std::string  content):
    Server(),
    _content(std::move(content))
  {}

  void
  _serve(std::unique_ptr<elle::reactor::network::Socket> socket) override
  {
    socket->write(this->_content);
  }

  ELLE_ATTRIBUTE_R(std::string, content);
};

static
void
read_until()
{
  elle::reactor::Scheduler sched;

  elle::reactor::Thread main(
    sched, "main",
    []
    {
      ContentServer server("foo\r\nx\r\nbar\r\n");

      elle::reactor::network::TCPSocket sock("127.0.0.1", server.port());
      BOOST_CHECK_EQUAL(sock.read_until("\r\n"), "foo\r\n");
      char c;
      sock.read(elle::WeakBuffer(&c, 1));
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
  elle::reactor::Scheduler sched;
  elle::reactor::Barrier listening;
  int port = 0;
  elle::reactor::Thread t(
    sched, "main",
    [&]
    {
      elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& scope)
      {
        scope.run_background(
          "server",
          [&]
          {
            elle::reactor::network::TCPServer server;
            server.listen(0);
            port = server.port();
            listening.open();
            std::unique_ptr<elle::reactor::network::Socket> socket(
              server.accept());
            socket->write(std::string("lulz"));
            socket->write(std::string("lol"));
          });
        scope.run_background(
          "client",
          [&]
          {
            elle::reactor::wait(listening);
            elle::reactor::network::TCPSocket socket("127.0.0.1", port);
            socket.write(std::string("lulz"));
            char data[1024];
            ELLE_LOG("first read")
              socket.std::iostream::read(data, 4);
            ELLE_LOG("second read")
              socket.std::iostream::read(data, 3);
          });
        elle::reactor::wait(scope);
      };
    });
  sched.run();
}

/*------------------------.
| Read / write cancelling |
`------------------------*/

ELLE_TEST_SCHEDULED(read_write_cancel)
{
  elle::reactor::Barrier listening;
  int port;
  elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& scope)
  {
    auto& server = scope.run_background(
      "server",
      [&]
      {
        elle::reactor::network::TCPServer server;
        server.listen();
        port = server.port();
        listening.open();
        auto client = server.accept();
        elle::reactor::sleep();
      });
    scope.run_background(
      "client",
      [&]
      {
        elle::reactor::wait(listening);
        elle::reactor::network::TCPSocket socket("127.0.0.1", port);
        elle::With<elle::reactor::Scope>() << [&] (elle::reactor::Scope& scope)
        {
          scope.run_background(
            "reader",
            [&]
            {
              BOOST_CHECK_THROW(socket.read(1, 1_sec),
                                elle::reactor::network::TimeOut);
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
          elle::reactor::wait(scope);
          server.terminate();
        };
      });
    elle::reactor::wait(scope);
  };
}

/*-----------------.
| Resolution abort |
`-----------------*/

ELLE_TEST_SCHEDULED(resolution_abort)
{
  elle::With<elle::reactor::Scope>() << [] (elle::reactor::Scope& scope)
  {
    auto& resolve = scope.run_background(
      "resolve",
      []
      {
        elle::reactor::network::resolve_tcp("bertier.lacogip.fr", "80");
      });
    scope.run_background(
      "kill",
      [&]
      {
        resolve.terminate_now();
      });
    elle::reactor::wait(scope);
  };
}

ELLE_TEST_SCHEDULED(read_terminate_recover)
{
  char wbuf[100];
  for (char & i : wbuf)
    i = rand();
  elle::reactor::network::TCPServer server;
  server.listen();
  elle::reactor::Barrier terminated;
  elle::reactor::Barrier reading;
  elle::reactor::Barrier written;
  elle::reactor::Thread accept(
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
        elle::reactor::wait(written);
        reading.open();
        socket->read(elle::WeakBuffer(buffer),
                     elle::DurationOpt(), &bytes_read);
      }
      catch (elle::reactor::Terminate const& e)
      {
        terminated.open();
        BOOST_CHECK_EQUAL(bytes_read, 50);
        BOOST_CHECK(!memcmp(buffer, wbuf, 50));
        socket->read(elle::WeakBuffer(buffer + bytes_read,
                                      sizeof buffer - bytes_read));
        BOOST_CHECK(!memcmp(buffer, wbuf, sizeof wbuf));
        throw;
      }
      BOOST_FAIL("unreachable");
    });
  elle::reactor::network::TCPSocket socket(
    "localhost", server.local_endpoint().port());
  ELLE_LOG("write 50")
    socket.write(elle::ConstWeakBuffer(wbuf, 50));
  written.open();
  elle::reactor::wait(reading);
  ELLE_LOG("kill reader")
    accept.terminate();
  elle::reactor::wait(terminated);
  ELLE_LOG("write 50")
    socket.write(elle::ConstWeakBuffer(wbuf+50, 50));
  elle::reactor::wait(accept);
}

ELLE_TEST_SCHEDULED(read_terminate_recover_iostream)
{
  char wbuf[100];
  for (char & i : wbuf)
    i = rand();
  elle::reactor::network::TCPServer server;
  server.listen();
  elle::reactor::Barrier terminated;
  elle::reactor::Barrier reading;
  elle::reactor::Thread accept(
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
      catch (elle::reactor::Terminate const& e)
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
  elle::reactor::network::TCPSocket socket(
    "localhost", server.local_endpoint().port());
  ELLE_LOG("write 50")
    socket.write(elle::ConstWeakBuffer(wbuf, 50));
  elle::reactor::wait(reading);
  ELLE_LOG("kill reader")
    accept.terminate();
  elle::reactor::wait(terminated);
  ELLE_LOG("write 50")
    socket.write(elle::ConstWeakBuffer(wbuf + 50, 50));
  socket.close();
  elle::reactor::wait(accept);
}

ELLE_TEST_SCHEDULED(read_terminate_deadlock)
{
  elle::reactor::network::TCPServer server;
  server.listen();
  elle::reactor::Barrier terminated;
  elle::reactor::Barrier reading;
  elle::reactor::Thread accept(
    "accept",
    [&]
    {
      auto socket = server.accept();
      char buffer[8] = {char(0xfd)};
      int read = 0;
      elle::IOStreamClear clearer(*socket);
      reading.open();
      while (read < 8)
        read += std::readsome(*socket, buffer + read, sizeof buffer - read);
      elle::reactor::sleep();
    });
  elle::reactor::network::TCPSocket socket(
    "localhost", server.local_endpoint().port());
  elle::reactor::wait(reading);
  ELLE_LOG("write 8")
    socket.write(elle::ConstWeakBuffer("abcdefgh", 8));
  accept.terminate_now();
}

ELLE_TEST_SCHEDULED(async_write)
{
  elle::reactor::network::TCPServer server;
  server.listen();
  elle::reactor::Barrier read;
  elle::reactor::Barrier written;
  elle::reactor::Thread accept(
    "accept",
    [&]
    {
      auto socket = server.accept();
      auto buffer = socket->read(9);
      BOOST_TEST(buffer == "foobarbaz");
      read.open();
    });
  elle::reactor::network::TCPSocket socket(
    "localhost", server.local_endpoint().port());
  boost::asio::deadline_timer t(elle::reactor::scheduler().io_service());
  t.expires_from_now(boost::posix_time::milliseconds(100));
  t.async_wait([&] (boost::system::error_code const& e)
               {
                 BOOST_TEST(!e);
                 socket.write("foo");
               });
  t.async_wait([&] (boost::system::error_code const& e)
               {
                 written.open();
                 BOOST_TEST(!e);
                 socket.write("bar");
               });
  elle::reactor::wait(written);
  socket.write("baz");
  elle::reactor::wait(read);
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
  suite.add(BOOST_TEST_CASE(async_write), 0, 10);
}
