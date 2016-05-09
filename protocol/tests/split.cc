#include <protocol/Serializer.hh>
#include <protocol/exceptions.hh>

#include <cryptography/random.hh>

#include <reactor/asio.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/tcp-server.hh>
#include <reactor/network/tcp-socket.hh>
#include <reactor/scheduler.hh>
#include <reactor/network/buffer.hh>
#include <elle/cast.hh>
#include <reactor/Scope.hh>
#include <reactor/semaphore.hh>
#include <reactor/thread.hh>

#include <elle/test.hh>
#include <elle/With.hh>
#include <elle/serialize/BinaryArchive.hh>

ELLE_LOG_COMPONENT("infinit.protocol.test");

static const elle::Version v010{0, 1, 0};
static const elle::Version v020{0, 2, 0};

typedef infinit::protocol::Serializer Serializer;

struct setup
{
  setup(elle::Version const& version,
        bool checksum = true)
  {
    reactor::network::TCPServer c;
    reactor::Barrier listening;
    int port = 0;
    elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
    {
      scope.run_background(
        "bob",
        [&]
        {
          reactor::network::TCPServer server;
          server.listen(0);
          port = server.port();
          listening.open();
          this->_bob.reset(server.accept().release());
          this->bob.reset(new Serializer(*this->_bob, version, checksum));
        });
      scope.run_background(
        "alice",
        [&]
        {
          reactor::wait(listening);
          this->_alice.reset(
            new reactor::network::TCPSocket("127.0.0.1", port));
          this->alice.reset(new Serializer(*this->_alice, version, checksum));
        });
      reactor::wait(scope);
    };
  }

public:
  std::unique_ptr<reactor::network::Socket> _alice;
  std::unique_ptr<reactor::network::Socket> _bob;
public:
  std::unique_ptr<Serializer> bob;
  std::unique_ptr<Serializer> alice;
};

static
void
exchange(Serializer& sender,
         Serializer& recipient,
         elle::Buffer const& input)
{
  // Same thread.
  {
    sender.write(input);
    auto output = recipient.read();
    ELLE_ASSERT_EQ(output.size(), input.size());
    ELLE_ASSERT_EQ(input, output);
  }
  // Different thread.
  {
    elle::Buffer output;
    elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
    {
      scope.run_background(
        "sender",
        [&]
        {
          sender.write(input);
        });
      scope.run_background(
        "recipient",
        [&]
        {
          output = recipient.read();
        });
      reactor::wait(scope);
    };
    ELLE_ASSERT_EQ(output.size(), input.size());
    ELLE_ASSERT_EQ(input, output);
  }
}

ELLE_TEST_SCHEDULED(run_v010)
{
  auto s = setup(v010);
  auto& bob = *s.bob;
  auto& alice = *s.alice;
  ELLE_ASSERT_EQ(bob.version(), v010);
  ELLE_ASSERT_EQ(alice.version(), v010);
  for (size_t size: {0, 1, 100, 10000, 100000})
  {
    exchange(
      bob, alice,
      infinit::cryptography::random::generate<std::string>(size));
    exchange(
      alice, bob,
      infinit::cryptography::random::generate<std::string>(size));
  }
}

ELLE_TEST_SCHEDULED(run_v020)
{
  auto s = setup(v020);
  auto& bob = *s.bob;
  auto& alice = *s.alice;
  ELLE_ASSERT_EQ(bob.version(), v020);
  ELLE_ASSERT_EQ(alice.version(), v020);
  for (size_t size: {0, 1, 100, 10000, 100000})
  {
    exchange(
      bob, alice,
      infinit::cryptography::random::generate<std::string>(size));
    exchange(
      alice, bob,
      infinit::cryptography::random::generate<std::string>(size));
  }
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(run_v010), 0, valgrind(5, 25));
  suite.add(BOOST_TEST_CASE(run_v020), 0, valgrind(5, 25));
}
