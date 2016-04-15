#include <protocol/exceptions.hh>
#include <protocol/Serializer.hh>

#include <cryptography/random.hh>

#include <reactor/asio.hh>
#include <reactor/Barrier.hh>
#include <reactor/Scope.hh>
#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/tcp-server.hh>
#include <reactor/scheduler.hh>

#include <elle/test.hh>
#include <elle/cast.hh>
#include <elle/With.hh>

ELLE_LOG_COMPONENT("infinit.protocol.test");

class SocketInstrumentation
{
public:
  struct Conf;

public:
  SocketInstrumentation()
    : _router(*reactor::Scheduler::scheduler(), "router",
              std::bind(&SocketInstrumentation::_route, this))
    , alice_conf(new Conf)
    , bob_conf(new Conf)
    , _alice_routed(0)
    , _bob_routed(0)
  {
    this->_a_server.listen();
    this->_b_server.listen();
    this->_alice.reset(
      new reactor::network::TCPSocket("127.0.0.1", this->_a_server.port()));
    this->_bob.reset(
      new reactor::network::TCPSocket("127.0.0.1", this->_b_server.port()));
    reactor::yield();
    reactor::yield();
  }

  ~SocketInstrumentation()
  {
    this->_router.terminate_now();
  }

  reactor::network::TCPSocket&
  alice()
  {
    return *this->_alice;
  }

  reactor::network::TCPSocket&
  bob()
  {
    return *this->_bob;
  }

  void
  alice_quota(size_t quota)
  {
    this->alice_conf->quota = quota;
  }

  void
  bob_quota(size_t quota)
  {
    this->bob_conf->quota = quota;
  }

  void
  alice_corrupt(size_t offset, char mask = 0xFF)
  {
    this->alice_conf->corrupt_offset = offset;
    this->alice_conf->corrupt_mask = mask;
  }

private:
  void
  _route()
  {
    using reactor::network::Buffer;

    std::unique_ptr<reactor::network::Socket> a(this->_a_server.accept());
    std::unique_ptr<reactor::network::Socket> b(this->_b_server.accept());

    elle::With<reactor::Scope>() << [&](reactor::Scope& scope)
    {
      auto route = [&] (reactor::network::Socket* sender,
                        reactor::network::Socket* recipient,
                        size_t& routed,
                        Conf& conf)
        {
          try
          {
            bool relay = true;
            while (relay)
            {
              char buffer[1024];
              ELLE_DEBUG("reading");
              size_t size = sender->read_some(Buffer(buffer, sizeof(buffer)),
                                              30_sec);
              ELLE_DEBUG("read %s", size);
              conf(elle::ConstWeakBuffer(buffer, size));
              if (conf.corrupt_offset >= 0 && \
                  conf.corrupt_offset >= routed && \
                  conf.corrupt_offset < routed + size)
              {
                int offset = conf.corrupt_offset - routed;
                ELLE_LOG("%s: corrupt byte %s", *this, offset);
                buffer[offset] ^= conf.corrupt_mask;
              }
              routed += size;
              conf(*this);
              if (conf.quota >= 0 && routed > conf.quota)
              {
                ELLE_LOG("%s: quota reached, interrupt", *this);
                size = size - (routed - conf.quota);
                relay = false;
                routed = conf.quota;
              }
              ELLE_TRACE("%s: route %s bytes", *this, size);
              recipient->write(elle::ConstWeakBuffer(buffer, size));
            }
          }
          catch (reactor::network::ConnectionClosed const&)
          {}
          sender->close();
          recipient->close();
        };
      scope.run_background("A to B",
                           std::bind(route, a.get(), b.get(),
                                     std::ref(this->_alice_routed),
                                     std::ref(*this->alice_conf)));
      scope.run_background("B to A",
                           std::bind(route, b.get(), a.get(),
                                     std::ref(this->_bob_routed),
                                     std::ref(*this->bob_conf)));
      scope.wait();
    };
  }

  reactor::network::TCPServer _a_server;
  reactor::network::TCPServer _b_server;
  std::unique_ptr<reactor::network::TCPSocket> _alice;
  std::unique_ptr<reactor::network::TCPSocket> _bob;
  reactor::Thread _router;

public:
  struct Conf
  {
    Conf():
      quota(-1),
      corrupt_offset(-1),
      corrupt_mask(0)
    {}

    virtual
    void
    operator ()(elle::ConstWeakBuffer const& data)
    {
    }

    virtual
    void
    operator ()(SocketInstrumentation const& sockets)
    {
    }

    size_t quota;
    size_t corrupt_offset;
    char corrupt_mask;
  };
public:
  std::unique_ptr<Conf> alice_conf;
  std::unique_ptr<Conf> bob_conf;
private:
  ELLE_ATTRIBUTE_RW(size_t, alice_routed);
  ELLE_ATTRIBUTE_RW(size_t, bob_routed);
};

typedef std::function<void (reactor::Thread&,
                            reactor::Thread&,
                            SocketInstrumentation&)> F;
static
void
dialog(elle::Version const& version,
       bool checksum,
       std::function<void (SocketInstrumentation&)> const& conf,
       std::function<void (infinit::protocol::Serializer&)> const& a,
       std::function<void (infinit::protocol::Serializer&)> const& b)
{
  reactor::Scheduler sched;

  reactor::Thread main(
    sched, "main",
    [&] ()
    {
      SocketInstrumentation sockets;
      std::unique_ptr<infinit::protocol::Serializer> alice;
      std::unique_ptr<infinit::protocol::Serializer> bob;
      elle::With<reactor::Scope>() << [&](reactor::Scope& scope)
      {
        scope.run_background(
          "setup alice's serializer",
          [&]
          {
            alice.reset(
              new infinit::protocol::Serializer(sockets.alice(), version, checksum));
          });
        scope.run_background(
          "setup bob's serializer",
          [&]
          {
            bob.reset(
              new infinit::protocol::Serializer(sockets.bob(), version, checksum));
          });
        scope.wait();
      };
      conf(sockets);
      elle::With<reactor::Scope>() << [&](reactor::Scope& scope)
      {
        scope.run_background("alice", boost::bind(a, boost::ref(*alice)));
        scope.run_background("bob", boost::bind(b, boost::ref(*bob)));
        scope.wait();
      };
    });

  sched.run();
}

#define CASES(function)                                                        \
  for (auto const& version: {elle::Version{0, 1, 0}, elle::Version{0, 2, 0}})  \
    for (auto checksum: {true, false})                                         \
      function(version, checksum)

static
void
_exchange_packets(elle::Version const& version,
                  bool checksum)
{
  dialog(version,
         checksum,
         [] (SocketInstrumentation&) {},
         [] (infinit::protocol::Serializer& s)
         {
           {
             elle::Buffer p("some data 42", strlen("some data 42"));
             s.write(p);
           }
           {
             elle::Buffer pp = s.read();
             elle::IOStream p(pp.istreambuf());
             std::string res;
             p >> res;
             BOOST_CHECK_EQUAL(res, "goody");
             BOOST_CHECK(!(p >> res));
           }
         },
         [] (infinit::protocol::Serializer& s)
         {
           {
             elle::Buffer pp = s.read();
             elle::IOStream p(pp.istreambuf());
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
             elle::Buffer p("goody", 5);
             s.write(p);
           }
         });
}

static
void
exchange_packets()
{
  CASES(_exchange_packets);
}

static
void
_exchange(elle::Version const& version,
          bool checksum)
{
  std::vector<elle::Buffer> packets={
    infinit::cryptography::random::generate<std::string>(0),
    infinit::cryptography::random::generate<std::string>(1),
    infinit::cryptography::random::generate<std::string>(100),
    infinit::cryptography::random::generate<std::string>(1000),
    infinit::cryptography::random::generate<std::string>(100000),
  };
  dialog(version,
         checksum,
         [] (SocketInstrumentation&) {},
         [&] (infinit::protocol::Serializer& s)
         {
           for (auto const& buffer: packets)
             s.write(buffer);
           for (size_t i = 0; i < packets.size(); ++i)
             BOOST_CHECK_EQUAL(s.read(), packets.at(i));
         },
         [&] (infinit::protocol::Serializer& s)
         {
           for (size_t i = 0; i < packets.size(); ++i)
             BOOST_CHECK_EQUAL(s.read(), packets.at(i));
           for (auto const& buffer: packets)
             s.write(buffer);
         });
}

static
void
exchange()
{
  CASES(_exchange);
}

static
void
_connection_lost_reader(elle::Version const& version,
                        bool checksum)
{
  dialog(
    version,
    checksum,
    [] (SocketInstrumentation& sockets)
    {
      // Reset 'routed' to ignore version exchange.
      sockets.alice_routed(0);
      sockets.bob_routed(0);
      sockets.alice_quota(96);
    },
    [] (infinit::protocol::Serializer& s)
    {
      elle::Buffer pp;
      elle::IOStream p(pp.ostreambuf());
      char buffer[1025];
      memset(buffer, 0xAA, sizeof(buffer));
      buffer[sizeof(buffer) - 1] = 0;
      p << buffer;
      p.flush();
      s.write(pp);
    },
    [] (infinit::protocol::Serializer& s)
    {
      BOOST_CHECK_THROW(s.read(), reactor::network::ConnectionClosed);
    });
}

static
void
connection_lost_reader()
{
  CASES(_connection_lost_reader);
}

static
void
_connection_lost_sender(elle::Version const& version,
                        bool checksum)
{
  dialog(
    version,
    checksum,
    [] (SocketInstrumentation& sockets)
    {
      // Reset 'routed' to ignore version exchange.
      sockets.alice_routed(0);
      sockets.bob_routed(0);
      sockets.alice_quota(4);
    },
    [] (infinit::protocol::Serializer& s)
    {
      elle::Buffer p("data", 4);
      s.write(p);
      // Getting an error from TCP writes is a bit touchy.
      try
      {
        while (true)
        {
          reactor::sleep(100_ms);
          s.write(p);
        }
      }
      catch (reactor::network::ConnectionClosed const&)
      {
        return;
      }
      BOOST_FAIL("ConnectionClosed exception was expected");
    },
    [] (infinit::protocol::Serializer&)
    {});
}

static
void
connection_lost_sender()
{
  CASES(_connection_lost_sender);
}

static
void
_corruption(elle::Version const& version,
            bool checksum)
{
  dialog(
    version,
    checksum,
    [] (SocketInstrumentation& sockets)
    {
      // Reset 'routed' to ignore version exchange.
      sockets.alice_routed(0);
      sockets.bob_routed(0);
      sockets.alice_corrupt(1024);
    },
    [] (infinit::protocol::Serializer& s)
    {
      elle::Buffer pp;
      elle::IOStream p(pp.ostreambuf());
      char buffer[1025];
      memset(buffer, 0xAA, sizeof(buffer));
      buffer[sizeof(buffer) - 1] = 0;
      p << buffer;
      p.flush();
      s.write(pp);
    },
    [=] (infinit::protocol::Serializer& s)
    {
      if (checksum)
        BOOST_CHECK_THROW(s.read(), infinit::protocol::ChecksumError);
    });
}

static
void
corruption()
{
  CASES(_corruption);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(exchange_packets), 0, valgrind(10, 10));
  suite.add(BOOST_TEST_CASE(exchange), 0, valgrind(15, 10));
  suite.add(BOOST_TEST_CASE(connection_lost_reader), 0, valgrind(3, 10));
  suite.add(BOOST_TEST_CASE(connection_lost_sender), 0, valgrind(3, 10));
  suite.add(BOOST_TEST_CASE(corruption), 0, valgrind(3, 10));
}
