#include <protocol/ChanneledStream.hh>
#include <protocol/RPC.hh>
#include <protocol/Serializer.hh>

#include <reactor/asio.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/tcp-server.hh>
#include <reactor/network/tcp-socket.hh>
#include <reactor/scheduler.hh>
#include <reactor/scheduler.hh>
#include <reactor/semaphore.hh>
#include <reactor/thread.hh>

#include <elle/test.hh>
#include <elle/serialize/BinaryArchive.hh>

ELLE_LOG_COMPONENT("infinit.protocol.test");

struct TestConfig
{
  bool sync;
  bool checksum;
  elle::Version version;
};

static
reactor::Thread* suicide_thread(nullptr);

struct DummyRPC:
  public infinit::protocol::RPC<elle::serialize::InputBinaryArchive,
                                elle::serialize::OutputBinaryArchive>
{
  DummyRPC(infinit::protocol::ChanneledStream& channels)
    : infinit::protocol::RPC<elle::serialize::InputBinaryArchive,
                             elle::serialize::OutputBinaryArchive>(channels)
    , answer("answer", *this)
    , square("square", *this)
    , concat("concat", *this)
    , raise("raise", *this)
    , suicide("suicide", *this)
    , count("count", *this)
    , wait("wait", *this)
  {}

  RemoteProcedure<int> answer;
  RemoteProcedure<int, int> square;
  RemoteProcedure<std::string, std::string const&, std::string const&> concat;
  RemoteProcedure<void> raise;
  RemoteProcedure<void> suicide;
  RemoteProcedure<int> count;
  RemoteProcedure<void> wait;
};

class RPCServer
{
public:
  RPCServer(TestConfig config)
    : _config(config)
    , _counter(0)
    , _server()
    , _thread(elle::sprintf("%s runner", *this), [this] { this->_run(); })
  {
    this->_server.listen();
  }

  ~RPCServer()
  {
    this->_thread.terminate_now();
  }

  int
  port() const
  {
    return this->_server.port();
  }

  void
  terminate()
  {
    this->_thread.terminate();
  }

  void
  _run()
  {
    auto& sched = *reactor::Scheduler::scheduler();
    auto socket = this->_server.accept();
    infinit::protocol::Serializer s(sched, *socket, _config.version, _config.checksum);
    infinit::protocol::ChanneledStream channels(sched, s);

    DummyRPC rpc(channels);
    rpc.answer = [] { return 42; };
    rpc.square = [] (int x) { return x * x; };
    rpc.concat = []
      (std::string const& a, std::string const& b) { return a + b; };
    rpc.raise = [] { throw std::runtime_error("blablabla"); };
    rpc.suicide = []
      {
        suicide_thread->terminate();
        suicide_thread = nullptr;
        reactor::yield();
        reactor::yield();
        reactor::yield();
        reactor::yield();
        reactor::yield();
        reactor::yield();
        BOOST_CHECK(false);
      };
    rpc.count =
      [this]
      {
        ++this->_counter;
        reactor::wait(this->_count_barrier);
        return this->_counter;
      };
    rpc.wait = [this] { ++this->_counter; reactor::sleep(); };
    try
    {
      if (this->_config.sync)
        rpc.run();
      else
        rpc.parallel_run();
    }
    catch (reactor::network::ConnectionClosed&)
    {}
  }

  ELLE_ATTRIBUTE_R(TestConfig, config);
  ELLE_ATTRIBUTE_R(int, counter);
  ELLE_ATTRIBUTE_RX(reactor::Barrier, count_barrier)
  ELLE_ATTRIBUTE(reactor::network::TCPServer, server);
  ELLE_ATTRIBUTE(reactor::Thread, thread);
};

/*------.
| Basic |
`------*/

ELLE_TEST_SCHEDULED(rpc, (TestConfig, config))
{
  RPCServer server(config);
  reactor::network::TCPSocket socket("127.0.0.1", server.port());
  infinit::protocol::Serializer s(socket, config.version, config.checksum);
  infinit::protocol::ChanneledStream channels(s);
  DummyRPC rpc(channels);
  BOOST_CHECK_EQUAL(rpc.answer(), 42);
  BOOST_CHECK_EQUAL(rpc.square(8), 64);
  BOOST_CHECK_EQUAL(rpc.concat("foo", "bar"), "foobar");
  BOOST_CHECK_THROW(rpc.raise(), std::runtime_error);
}

/*----------.
| Terminate |
`----------*/

ELLE_TEST_SCHEDULED(terminate, (TestConfig, config))
{
  RPCServer server(config);
  reactor::Thread thread(
    "judge dread",
    [&]
    {
      reactor::network::TCPSocket socket("127.0.0.1", server.port());
      infinit::protocol::Serializer s(socket, config.version, config.checksum);
      infinit::protocol::ChanneledStream channels(s);
      DummyRPC rpc(channels);
      suicide_thread = &thread;
      BOOST_CHECK_THROW(rpc.suicide(), std::runtime_error);
    });
  reactor::wait(thread);
}

/*---------.
| Parallel |
`---------*/

ELLE_TEST_SCHEDULED(parallel, (TestConfig, config))
{
  RPCServer server(config);
  reactor::network::TCPSocket socket("127.0.0.1", server.port());
  infinit::protocol::Serializer s(socket, config.version, config.checksum);
  infinit::protocol::ChanneledStream channels(s);
  DummyRPC rpc(channels);
  std::vector<reactor::Thread*> threads;
  std::list<int> inserted;
  elle::With<reactor::Scope>() << [&] (reactor::Scope& scope)
  {
    for (int i = 1; i <= 3; ++i)
    {
      if (config.sync)
        inserted.push_back(i);
      scope.run_background(
        elle::sprintf("counter %s", i),
        [&]
        {
          if (config.sync)
            inserted.remove(rpc.count());
          else
            BOOST_CHECK_EQUAL(rpc.count(), 3);
        });
    }
    // Let the RPC get executed once.
    if (!config.sync)
      do
      {
        reactor::yield();
      }
      while (server.counter() != 3);
    server.count_barrier().open();
    reactor::wait(scope);
  };
  BOOST_CHECK(inserted.empty());
}

/*--------------.
| Disconnection |
`--------------*/

ELLE_TEST_SCHEDULED(disconnection, (TestConfig, config))
{
  RPCServer server(config);
  reactor::network::TCPSocket socket("127.0.0.1", server.port());
  infinit::protocol::Serializer s(socket, config.version, config.checksum);
  infinit::protocol::ChanneledStream channels(s);
  DummyRPC rpc(channels);
  reactor::Thread call_1("call 1",
                         [&]
                         {
                           BOOST_CHECK_THROW(rpc.wait(), std::runtime_error);
                         });
  reactor::Thread call_2("call 2",
                         [&]
                         {
                           BOOST_CHECK_THROW(rpc.wait(), std::runtime_error);
                         });
  do
  {
    reactor::yield();
  }
  while (server.counter() < (config.sync ? 1 : 2));
  server.terminate();
  reactor::wait({&call_1, &call_2});
}

/*-----------.
| Test suite |
`-----------*/

#define ELLE_TEST_NAMED_CASE(func, name)\
  ::boost::unit_test::make_test_case(func, name)

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  TestConfig configs[] = {
    {true,  false, elle::Version(0, 1, 0)},
    {true,  false, elle::Version(0, 2, 0)},
    {true,  true,  elle::Version(0, 1, 0)},
    {true,  true,  elle::Version(0, 2, 0)},
    {false, true,  elle::Version(0, 1, 0)},
    {false, true,  elle::Version(0, 2, 0)},
    {false, false, elle::Version(0, 1, 0)},
    {false, false, elle::Version(0, 2, 0)},
  };
  auto test = [&](std::string const& name, std::function<void(TestConfig)> f)
  {
    for (int i=0; i<8; ++i)
      suite.add(
        BOOST_TEST_CASE(std::bind(f, configs[i])), 0, valgrind(1, 10));
  };
  test("rpc", &rpc);
  test("terminate", &terminate);
  test("parallel", &parallel);
  test("disconnection", &disconnection);
}
