#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <elle/With.hh>

#include <reactor/Barrier.hh>
#include <reactor/Scope.hh>
#include <reactor/scheduler.hh>
#include <reactor/network/tcp-server.hh>

#include <protocol/Serializer.hh>

/// Helper to connect two serializers.
class Serializers
{
public:
  Serializers():
    _server()
  {
    auto& sched = *reactor::Scheduler::scheduler();
    elle::With<reactor::Scope>() << [&](reactor::Scope& scope)
    {
      reactor::Barrier listening;
      int port = 0;
      scope.run_background(
        "server", [&]
        {
          this->_server.listen();
          port = this->_server.port();
          listening.open();
          this->_server_socket.reset(this->_server.accept());
          this->alice.reset(
            new infinit::protocol::Serializer(*this->_server_socket));
        });
      scope.run_background(
        "client", [&]
        {
          listening.wait();
          this->_client_socket.reset(
            new reactor::network::TCPSocket(sched, "127.0.0.1", port));
          this->bob.reset(
            new infinit::protocol::Serializer(*this->_client_socket));
        });
      scope.wait();
    };
  }
  std::unique_ptr<infinit::protocol::Serializer> alice;
  std::unique_ptr<infinit::protocol::Serializer> bob;

private:
  reactor::network::TCPServer _server;
  std::unique_ptr<reactor::network::TCPSocket> _server_socket;
  std::unique_ptr<reactor::network::TCPSocket> _client_socket;
};

static
void
exchange_packets()
{
  reactor::Scheduler sched;

  reactor::Thread main(
    sched, "main",
    [&] ()
    {
      Serializers serializers;
      auto& alice = *serializers.alice;
      auto& bob = *serializers.bob;

      elle::With<reactor::Scope>() << [&](reactor::Scope& scope)
      {
        scope.run_background(
          "alice", [&]
          {
            {
              {
                infinit::protocol::Packet p;
                p << "some data 42";
                alice.write(p);
              }
              {
                infinit::protocol::Packet p = alice.read();
                std::string res;
                p >> res;
                BOOST_CHECK_EQUAL(res, "goody");
                BOOST_CHECK(!(p >> res));
              }
            }
          });
        scope.run_background(
          "bob", [&]
          {
            {
              infinit::protocol::Packet p = bob.read();
              std::string some, data;
              int i;
              p >> some;
              BOOST_CHECK_EQUAL(some, "some");
              p >> data;
              BOOST_CHECK_EQUAL(data, "data");
              p >> i;
              BOOST_CHECK_EQUAL(i, 42);
              BOOST_CHECK(!(p >> data));
            }
            {
              infinit::protocol::Packet p;
              p << "goody";
              bob.write(p);
            }
          });
        scope.wait();
      };
    });

  sched.run();
}

static
void
exchange_packets()
{
  reactor::Scheduler sched;

  reactor::Thread main(
    sched, "main",
    [&] ()
    {
      Serializers serializers;
      auto& alice = *serializers.alice;
      auto& bob = *serializers.bob;

      elle::With<reactor::Scope>() << [&](reactor::Scope& scope)
      {
        scope.run_background(
          "alice", [&]
          {
            {
              {
                infinit::protocol::Packet p;
                p << "some data 42";
                alice.write(p);
              }
              {
                infinit::protocol::Packet p = alice.read();
                std::string res;
                p >> res;
                BOOST_CHECK_EQUAL(res, "goody");
                BOOST_CHECK(!(p >> res));
              }
            }
          });
        scope.run_background(
          "bob", [&]
          {
            {
              infinit::protocol::Packet p = bob.read();
              std::string some, data;
              int i;
              p >> some;
              BOOST_CHECK_EQUAL(some, "some");
              p >> data;
              BOOST_CHECK_EQUAL(data, "data");
              p >> i;
              BOOST_CHECK_EQUAL(i, 42);
              BOOST_CHECK(!(p >> data));
            }
            {
              infinit::protocol::Packet p;
              p << "goody";
              bob.write(p);
            }
          });
        scope.wait();
      };
    });

  sched.run();
}

static
bool
test_suite()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(exchange_packets), 0, 3);
  return true;
}

int
main(int argc, char** argv)
{
  return ::boost::unit_test::unit_test_main(test_suite, argc, argv);
}
