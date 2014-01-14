#include <protocol/ChanneledStream.hh>
#include <protocol/RPC.hh>
#include <protocol/Serializer.hh>

#include <reactor/asio.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/tcp-server.hh>
#include <reactor/scheduler.hh>
#include <reactor/scheduler.hh>
#include <reactor/semaphore.hh>
#include <reactor/thread.hh>

#include <elle/test.hh>
#include <elle/serialize/BinaryArchive.hh>

static
int
answer()
{
  return 42;
}

static
int
square(int x)
{
  return x * x;
}

static
std::string
concat(std::string const& left,
       std::string const& right)
{
  return left + right;
}

static
reactor::Thread* suicide_thread(nullptr);

static
void
suicide()
{
  suicide_thread->terminate();
  suicide_thread = nullptr;
  reactor::Scheduler::scheduler()->current()->yield();
  reactor::Scheduler::scheduler()->current()->yield();
  BOOST_CHECK(false);
}

static int global_counter = 0;

static
int
count()
{
  ++global_counter;
  // This gives 100ms to the rpc to "synchronize".
  reactor::Scheduler::scheduler()->current()->sleep(
    boost::posix_time::milliseconds(1000));
  return global_counter;
}

static
void
except()
{
  throw std::runtime_error("blablabla");
}

static
void
await()
{
  reactor::Scheduler::scheduler()->current()->sleep(
    boost::posix_time::seconds(2));
}

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

/*------.
| Basic |
`------*/

static
void
caller(reactor::Semaphore& lock, int& port)
{
  auto& sched = *reactor::Scheduler::scheduler();
  sched.current()->wait(lock);
  reactor::network::TCPSocket socket(sched, "127.0.0.1", port);
  infinit::protocol::Serializer s(sched, socket);
  infinit::protocol::ChanneledStream channels(sched, s);

  DummyRPC rpc(channels);
  BOOST_CHECK_EQUAL(rpc.answer(), 42);
  BOOST_CHECK_EQUAL(rpc.square(8), 64);
  BOOST_CHECK_EQUAL(rpc.concat("foo", "bar"), "foobar");
  BOOST_CHECK_THROW(rpc.raise(), std::runtime_error);
}

static
void
runner(reactor::Semaphore& lock,
       bool sync,
       int& port)
{
  auto& sched = *reactor::Scheduler::scheduler();
  reactor::network::TCPServer server(sched);
  server.listen();
  port = server.port();
  lock.release();
  lock.release();
  std::unique_ptr<reactor::network::TCPSocket> socket(server.accept());
  infinit::protocol::Serializer s(sched, *socket);
  infinit::protocol::ChanneledStream channels(sched, s);

  DummyRPC rpc(channels);
  rpc.answer = &answer;
  rpc.square = &square;
  rpc.concat = &concat;
  rpc.raise = &except;
  rpc.suicide = &suicide;
  rpc.count = &count;
  rpc.wait = &await;
  try
  {
    if (sync)
      rpc.run();
    else
      rpc.parallel_run();
  }
  catch (reactor::network::ConnectionClosed&)
  {}
}

static
int
rpc(bool sync)
{
  reactor::Scheduler sched;
  reactor::Semaphore lock;
  int port = 0;

  reactor::Thread r(sched, "Runner",
                    std::bind(runner,
                              std::ref(lock),
                              sync,
                              std::ref(port)));
  reactor::Thread c(sched, "Caller",
                    std::bind(caller,
                              std::ref(lock),
                              std::ref(port)));

  sched.run();
  return 0;
}

/*----------.
| Terminate |
`----------*/

static
void
pacify(reactor::Semaphore& lock,
       reactor::Thread& t,
       int& port)
{
  auto& sched = *reactor::Scheduler::scheduler();
  sched.current()->wait(lock);
  reactor::network::TCPSocket socket(sched, "127.0.0.1", port);
  infinit::protocol::Serializer s(sched, socket);
  infinit::protocol::ChanneledStream channels(sched, s);

  DummyRPC rpc(channels);
  suicide_thread = &t;
  BOOST_CHECK_THROW(rpc.suicide(), std::runtime_error);
}

static
int
terminate(bool sync)
{
  reactor::Scheduler sched{};
  reactor::Semaphore lock;
  int port = 0;

  reactor::Thread r(sched, "Runner", std::bind(runner,
                                               std::ref(lock),
                                               sync,
                                               std::ref(port)));
  reactor::Thread j(sched, "Judge dread", std::bind(pacify,
                                                    std::ref(lock),
                                                    std::ref(r),
                                                    std::ref(port)));

  sched.run();
  return 0;
}

/*---------.
| Parallel |
`---------*/

static
void
counter(reactor::Semaphore& lock,
        bool sync,
        int& port)
{
  auto& sched = *reactor::Scheduler::scheduler();
  sched.current()->wait(lock);
  reactor::network::TCPSocket socket(sched, "127.0.0.1", port);
  infinit::protocol::Serializer s(sched, socket);
  infinit::protocol::ChanneledStream channels(sched, s);

  DummyRPC rpc(channels);
  global_counter = 0;
  std::vector<reactor::Thread*> threads;
  std::list<int> inserted;
  for (int i = 1; i <= 3; ++i)
  {
    if (sync)
      inserted.push_back(i);
    auto t = new reactor::Thread(sched,
                                 elle::sprintf("Counter %s", i),
                                 [&,i] ()
                                 {
                                   if (sync)
                                     inserted.remove(rpc.count());
                                   else
                                     BOOST_CHECK_EQUAL(rpc.count(), 3);
                                 });
    threads.push_back(t);
  }
  for (auto& t: threads)
  {
    sched.current()->wait(*t);
    delete t;
  }

  BOOST_CHECK(inserted.empty());
}


static
int
parallel(bool sync)
{
  reactor::Scheduler sched;
  reactor::Semaphore lock;
  int port = 0;

  reactor::Thread r(sched, "Runner", std::bind(runner,
                                               std::ref(lock),
                                               sync,
                                               std::ref(port)));
  reactor::Thread c1(sched, "Counter", std::bind(counter,
                                                 std::ref(lock),
                                                 sync,
                                                 std::ref(port)));

  sched.run();
  return 0;
}

/*--------------.
| Disconnection |
`--------------*/

static
void
disconnection_caller(reactor::Semaphore& lock,
                       bool,
                       int& port)
{
  auto& sched = *reactor::Scheduler::scheduler();
  sched.current()->wait(lock);
  reactor::network::TCPSocket socket(sched, "127.0.0.1", port);
  infinit::protocol::Serializer s(sched, socket);
  infinit::protocol::ChanneledStream channels(sched, s);

  DummyRPC rpc(channels);

  reactor::Thread call_1(sched, "Call 1", [&]() {
      BOOST_CHECK_THROW(rpc.wait(), std::runtime_error);
    });
  reactor::Thread call_2(sched, "Call 2", [&]() {
      BOOST_CHECK_THROW(rpc.wait(), std::runtime_error);
    });
  sched.current()->wait({&call_1, &call_2});
}

static
int
disconnection(bool sync)
{
  reactor::Scheduler sched;
  reactor::Semaphore lock;
  int port = 0;

  reactor::Thread r(sched, "Runner", std::bind(runner,
                                               std::ref(lock),
                                               true,
                                               std::ref(port)));
  reactor::Thread c1(sched, "Caller", std::bind(disconnection_caller,
                                                  std::ref(lock),
                                                  sync,
                                                  std::ref(port)));
  reactor::Thread killer(sched, "Killer", [&] {
      reactor::Scheduler::scheduler()->current()->sleep(
        boost::posix_time::seconds(1));
      r.terminate();
    });


  sched.run();
  return 0;
}

/*-----------.
| Test suite |
`-----------*/

#define ELLE_TEST_NAMED_CASE(func, name)\
  ::boost::unit_test::make_test_case(func, name)

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
#define TEST(Name)                                                      \
  auto BOOST_PP_CAT(Name, _sync) = std::bind(Name, true);               \
  suite.add(BOOST_TEST_CASE(BOOST_PP_CAT(Name, _sync)), 0, 5);          \
  auto BOOST_PP_CAT(Name, _async) = std::bind(Name, false);             \
  suite.add(BOOST_TEST_CASE(BOOST_PP_CAT(Name, _async)), 0, 5);         \

  TEST(rpc);
  TEST(terminate);
  TEST(parallel);
  TEST(disconnection);
}
