#include <elle/With.hh>
#include <elle/Buffer.hh>
#include <elle/cast.hh>
#include <elle/test.hh>

#include <elle/cryptography/random.hh>

#include <reactor/Scope.hh>
#include <reactor/asio.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/tcp-server.hh>
#include <reactor/network/tcp-socket.hh>
#include <reactor/scheduler.hh>
#include <reactor/semaphore.hh>
#include <reactor/thread.hh>

#include <elle/protocol/Serializer.hh>
#include <elle/protocol/exceptions.hh>
#include <elle/protocol/Channel.hh>
#include <elle/protocol/ChanneledStream.hh>

ELLE_LOG_COMPONENT("elle.protocol.test");

using Serializer = elle::protocol::Serializer;

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

ELLE_TEST_SCHEDULED(run_version, (elle::Version, version))
{
  auto s = setup(version);
  auto& bob = *s.bob;
  auto& alice = *s.alice;
  ELLE_ASSERT_EQ(bob.version(), version);
  ELLE_ASSERT_EQ(alice.version(), version);
  for (size_t size: {0, 1, 100, 10000, 100000})
  {
    exchange(
      bob, alice,
      elle::cryptography::random::generate<std::string>(size));
    exchange(
      alice, bob,
      elle::cryptography::random::generate<std::string>(size));
  }
}

ELLE_TEST_SCHEDULED(kill_reader)
{
  /* When channeledstream's effective reader is terminated, it
   * used to wake a single thread that was waiting on the stream using signal().
   * So if that thread terminates at that exact momemnt, there is no more
   * any reader on the stream.
  */
  using namespace elle::protocol;
  auto v = elle::Version(0,2,0);
  reactor::Barrier b;
  reactor::network::TCPServer srv;
  srv.listen(0);
  std::unique_ptr<reactor::network::TCPSocket> s2;
  std::unique_ptr<Serializer> ser2p;
  std::unique_ptr<ChanneledStream> cs2p;
  new reactor::Thread("accept", [&] {
      s2 = srv.accept();
      ser2p.reset(new Serializer(*s2, v, false));
      cs2p.reset(new ChanneledStream(*ser2p));
  }, true);

  reactor::network::TCPSocket s1("127.0.0.1",
                                 srv.local_endpoint().port());
  Serializer ser1(s1, v, false);
  ChanneledStream cs1(ser1);
  while (!cs2p)
    reactor::sleep(50_ms);
  ChanneledStream& cs2 = *cs2p;

  // FIXME: unused.
  int cid1, cid2, cid3;
  bool r1 = false, r2 = false, r3 = false;
  reactor::Thread::unique_ptr t1, t2, t3;
  t1.reset(new reactor::Thread("t1", [&] {
      elle::protocol::Channel c(cs1);
      b.open();
      cid1 = c.id();
      try {
        c.read();
      }
      catch (std::exception const& e)
      {
        t3->terminate_now();
        throw;
      }
      r1 = true;
  }));
  reactor::wait(b);
  b.close();
  //t1 is now the channel listener
  t2.reset(new reactor::Thread("t2", [&] {
      elle::protocol::Channel c(cs1);
      b.open();
      cid2 = c.id();
      c.write(elle::Buffer("foo"));
      c.read();
      r2 = true;
  }));
  // t2 is waiting
  reactor::wait(b);
  b.close();
  t3.reset(new reactor::Thread("t3", [&] {
      elle::protocol::Channel c(cs1);
      b.open();
      cid3 = c.id();
      c.read();
      r3 = true;
  }));
  reactor::wait(b);
  // t1 will kill t3, but it also works when killing from outside
  t1->terminate_now();
  // ensure t2 took over reading
  Channel c = cs2.accept();
  c.write(elle::Buffer("foo"));
  while (!r2)
    reactor::sleep(100_ms);
  BOOST_CHECK(r2);
}

ELLE_TEST_SCHEDULED(nonempty_queue)
{
  using namespace elle::protocol;
  auto v = elle::Version(0,2,0);
  reactor::Barrier b1,b2;
  reactor::network::TCPServer srv;
  srv.listen(0);
  std::unique_ptr<reactor::network::TCPSocket> s2;
  std::unique_ptr<Serializer> ser2p;
  std::unique_ptr<ChanneledStream> cs2p;
  new reactor::Thread("accept", [&] {
      s2 = srv.accept();
      ser2p.reset(new Serializer(*s2, v, false));
      cs2p.reset(new ChanneledStream(*ser2p));
  }, true);

  reactor::network::TCPSocket s1("127.0.0.1",
                                 srv.local_endpoint().port());
  Serializer ser1(s1, v, false);
  ChanneledStream cs1(ser1);
  while (!cs2p)
    reactor::sleep(50_ms);
  ChanneledStream& cs2 = *cs2p;
  // force a reader on cs1
  reactor::Thread::unique_ptr reader(new reactor::Thread("reader",
    [&]
    {
      Channel r(cs1);
      r.read();
      ELLE_ASSERT(false);
    }));

  // accept first
  {
    new reactor::Thread("new_chan", [&] {
        b1.open();
        cs1.accept().read();
        b2.open();
    }, true);
    reactor::wait(b1);
    auto chan = Channel(cs2);
    chan.write(elle::Buffer("foo"));
    BOOST_CHECK(reactor::wait(b2, 1_sec));
  }
  //write first
  {
    b1.close();
    b2.close();
    auto chan = Channel(cs2);
    chan.write(elle::Buffer("bar"));
    new reactor::Thread("new_chan", [&] {
        b1.open();
        cs1.accept().read();
        b2.open();
    }, true);
    BOOST_CHECK(reactor::wait(b2, 1_sec));
  }
}


ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  using versions = std::initializer_list<elle::Version>;
  for (auto const& v: versions{{0, 1, 0}, {0, 2, 0}})
    suite.add(BOOST_TEST_CASE(std::bind(run_version, v)), 0, valgrind(5, 25));
  suite.add(BOOST_TEST_CASE(kill_reader), 0, valgrind(5, 25));
  suite.add(BOOST_TEST_CASE(nonempty_queue), 0, valgrind(5, 25));
}
