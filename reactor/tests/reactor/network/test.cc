#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/tcp-server.hh>
#include <reactor/network/udp-server.hh>
#include <reactor/network/udp-server-socket.hh>
#include <reactor/network/socket.hh>
#include <reactor/signal.hh>
#include <reactor/thread.hh>

#include "test.hh"

using reactor::network::Byte;
using reactor::network::Buffer;
using reactor::Signal;
using reactor::network::Size;
using reactor::network::TCPSocket;
using reactor::network::TCPServer;
using reactor::network::UDPSocket;
using reactor::network::UDPServer;
using reactor::Thread;

/*---------------.
| Destroy socket |
`---------------*/

void test_destroy_socket_non_connected()
{
  // Used to fail because shuting down a socket whose connection
  // failed used to raise.
  try
  {
    TCPSocket socket(*sched, "127.0.0.1", 4242);
  }
  catch (...)
  {
    BOOST_CHECK(true);
    return;
  }
  BOOST_FAIL("Socket shouldn't have connected");
}

void test_destroy_socket()
{
  sched = new reactor::Scheduler;
  Thread t(*sched, "thread", &test_destroy_socket_non_connected);
  sched->run();
  delete sched;
}

/*---------.
| Timeouts |
`---------*/

template <typename Server, typename Socket>
void silent_server()
{
  Server server(*sched);
  server.listen(4242);
  reactor::network::Socket* socket = server.accept();
  sched->current()->sleep(boost::posix_time::milliseconds(400));
  socket->write("0");
  delete socket;
}

template <typename Server, typename Socket>
void timeout_read()
{
  Socket socket(*sched, "127.0.0.1", 4242);
  // Poke the server to establish the pseudo connection in the UDP case.
  socket.write("poke");
  Byte b;
  reactor::network::Buffer buffer(&b, 1);
  BOOST_CHECK_THROW(
    socket.read_some(buffer, boost::posix_time::milliseconds(200)),
    reactor::network::TimeOut);
  try
  {
    socket.read_some(buffer, boost::posix_time::milliseconds(400));
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

  reactor::Thread server(*sched, "server", &silent_server<Server, Socket>);
  reactor::Thread client(*sched, "read", &timeout_read<Server, Socket>);
  sched->run();
}

/*------------.
| Echo Server |
`------------*/

template <typename Server, typename Socket>
void server();
void serve(reactor::network::Socket* socket);
template <typename Server, typename Socket>
void client(std::vector<std::string> messages);

template <typename Server, typename Socket>
void server()
{
  Server server(*sched);
  server.listen(4242);
  int nclients = 2;
  reactor::Waitables clients;
  while (nclients--)
  {
    reactor::network::Socket* socket = server.accept();
    clients << new reactor::Thread(*sched, "serve",
                                   boost::bind(serve, socket));
  }
  sched->current()->wait(clients);
  BOOST_FOREACH(reactor::Waitable* thread, clients)
    delete thread;
}

void serve(reactor::network::Socket* socket)
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
      socket->write(Buffer(received.c_str(), pos + 1));
      received = received.substr(pos + 1);
    }
  }
}

template <typename Server, typename Socket>
void client(std::vector<std::string> messages, unsigned& check)
{
  Socket s(*sched, "127.0.0.1", 4242);
  BOOST_FOREACH (const std::string& message, messages)
  {
    s.write(Buffer(message));

    Byte buf[256];
    Size read = s.read_some(Buffer(buf, 256));
    buf[read] = 0;
    BOOST_CHECK_EQUAL(message, reinterpret_cast<char*>(buf));
    ++check;
  }
}

template <typename Server, typename Socket>
void test_echo_server()
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

namespace reactor
{
  namespace network
  {
    boost::unit_test::test_suite* test_suite()
    {
      boost::unit_test::test_suite* network = BOOST_TEST_SUITE("Network");
      network->add(BOOST_TEST_CASE(test_destroy_socket));
#define INFINIT_REACTOR_NETWORK_TEST(Proto)                             \
      network->add(BOOST_TEST_CASE((test_timeout_read                   \
                                    <Proto##Server, Proto##Socket>)));  \
      network->add(BOOST_TEST_CASE((test_echo_server                    \
                                    <Proto##Server, Proto##Socket>)));  \

      INFINIT_REACTOR_NETWORK_TEST(TCP);
      INFINIT_REACTOR_NETWORK_TEST(UDP);
#undef INFINIT_REACTOR_NETWORK_TEST
      return network;
    }
  }
}
