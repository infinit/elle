#include <reactor/scheduler.hh>
#include <reactor/network/utp-socket.hh>
#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>
#include <elle/test.hh>
#include <elle/assert.hh>

ELLE_LOG_COMPONENT("utpcat");

using namespace reactor::network;

ELLE_TEST_SCHEDULED(udp)
{
  reactor::network::UDPSocket s;
  s.socket()->close();
  s.bind(boost::asio::ip::udp::endpoint({}, 0));
  auto t = new reactor::Thread("recv", [&] {
      char data[1000];
      boost::asio::ip::udp::endpoint ep;
      s.receive_from(Buffer(data, 1000), ep);
  });
  reactor::sleep(100_ms);
  t->terminate();
  reactor::wait(*t);
  delete t;
}


class SocketPair
{
public:
  SocketPair();
  reactor::network::UTPServer srv1, srv2;
  std::unique_ptr<reactor::network::UTPSocket> s1, s2;
};

ELLE_TEST_SCHEDULED(basic)
{
  {
    SocketPair sp;
    sp.s1->write("foo");
    ELLE_ASSERT_EQ(sp.s2->read_some(3).string(), "foo");
    sp.s2->write("bar");
    ELLE_ASSERT(sp.s1->read_some(3).string() == "bar");
    sp.s1->write("baz");
    sp.s2->write("bam");
    ELLE_ASSERT(sp.s1->read_some(3).string() == "bam");
    ELLE_ASSERT(sp.s2->read_some(3).string() == "baz");
    ELLE_LOG("done");
  }
  ELLE_LOG("done2");
}

ELLE_TEST_SCHEDULED(utp_close)
{
  SocketPair sp;
  sp.s1->write("foo");
  sp.s1->close();
  reactor::sleep(100_ms);
  BOOST_CHECK_THROW(sp.s1->write("bar"), reactor::network::SocketClosed);
  BOOST_CHECK_THROW(sp.s1->read_some(10), reactor::network::SocketClosed);
  BOOST_CHECK_THROW(sp.s2->read_some(10), reactor::network::SocketClosed);
  BOOST_CHECK_THROW(sp.s2->write("bar"), reactor::network::SocketClosed);
  {
    UTPServer srv;
    reactor::network::UTPSocket sock(srv);
  }
  {
    SocketPair sp;
  }
  {
    UTPServer srv1, srv2;
    reactor::Thread* t;
    srv1.listen(0); srv2.listen(0);
    {
      auto s1 = elle::make_unique<reactor::network::UTPSocket>(srv2);
      reactor::Barrier b;
      t = new reactor::Thread ("foo", [&] {
          b.open();
          try {
            s1->connect("127.0.0.1", srv1.local_endpoint().port());
          }
          catch(...)
          {}
      });
      b.wait();
    }
    t->terminate_now();
    delete t;
  }
}

ELLE_TEST_SCHEDULED(utp_timeout)
{
  SocketPair sp;
  BOOST_CHECK_THROW(sp.s1->read(10, 100_ms), reactor::network::TimeOut);
  sp.s2->write("foooo");
  BOOST_CHECK_THROW(sp.s1->read(10, 100_ms), reactor::network::TimeOut);
  char* megabuf = (char*)malloc(10000000);
  memset(megabuf, 0, 10000000);
  BOOST_CHECK_THROW(sp.s1->write(elle::ConstWeakBuffer(megabuf, 10000000),
                                 100_ms),
                    reactor::network::TimeOut);
  free(megabuf);
}

ELLE_TEST_SCHEDULED(utp_failures)
{
  SocketPair sp;
  reactor::network::UTPSocket socket(sp.srv1);
  ELLE_LOG("connect");
  BOOST_CHECK_THROW(socket.connect("127.0.0.1", 65530), SocketClosed);
  // check other sockets are unaffected
  sp.s1->write("foo");
  ELLE_LOG("read");
  ELLE_ASSERT_EQ(sp.s2->read(3), "foo");
}

ELLE_TEST_SCHEDULED(streams)
{
  SocketPair sp;
  (*sp.s1) << "foo " << 12 << std::endl;
  std::string s;
  int i;
  (*sp.s2) >> s >> i;
  ELLE_ASSERT_EQ(s, "foo");
  ELLE_ASSERT_EQ(i, 12);
}

ELLE_TEST_SCHEDULED(big)
{
  SocketPair sp;
  size_t buffer_size = 20000u;
  std::string data(buffer_size, '-');
  // note: there is no garantee utp will buffer that much
  sp.s1->write(data);
  elle::Buffer buf = sp.s2->read(buffer_size);
  ELLE_ASSERT_EQ(buf.size(), buffer_size);
}

ELLE_TEST_SCHEDULED(many)
{
  SocketPair sp;
  std::string data(1000, '-');
  int iw1 = 0;
  int ir2 = 0;
  auto const iterations = 100;
  reactor::Thread::unique_ptr w1(
    new reactor::Thread("writer 1", [&] {
        for (iw1 = 0; iw1 < iterations; ++iw1)
          sp.s1->reactor::network::UTPSocket::write(elle::ConstWeakBuffer(data));
      }));
  reactor::Thread::unique_ptr r2(
    new reactor::Thread("reader 2", [&] {
        for (ir2 = 0; ir2 < iterations; ++ir2)
          sp.s2->read(1000);
      }));
  reactor::Thread::unique_ptr watch(
    new reactor::Thread("watch", [&] {
        while (true)
        {
          reactor::sleep(500_ms);
          std::cerr << "**I: " << iw1 << ' ' << ir2 << std::endl;
        }
      }));
  reactor::wait(*w1);
  reactor::wait(*r2);
  sp.s1->stats();
  sp.s2->stats();
}

SocketPair::SocketPair()
{
  srv1.listen(0);
  srv2.listen(0);
  s1 = elle::make_unique<reactor::network::UTPSocket>(srv2);
  s1->connect("127.0.0.1",
              srv1.local_endpoint().port());
  ELLE_LOG("SocketPair accepting");
  s2 = srv1.accept();
  ELLE_LOG("SocketPair ready");
}

static
void
loop_socket(reactor::network::UTPSocket* ts)
{
  std::unique_ptr<reactor::network::UTPSocket> s(ts);
  while (true)
  {
    elle::Buffer buf = s->read(4000);
    ELLE_LOG("pong %s", buf.size());
    s->write(buf);
  }
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(udp), 0, valgrind(2));
  suite.add(BOOST_TEST_CASE(utp_close), 0, valgrind(2));
  suite.add(BOOST_TEST_CASE(basic), 0, valgrind(2));
  suite.add(BOOST_TEST_CASE(utp_timeout), 0, valgrind(2));
#ifdef INFINIT_LINUX
  suite.add(BOOST_TEST_CASE(utp_failures), 0, valgrind(2));
#endif
  suite.add(BOOST_TEST_CASE(streams), 0, valgrind(2));
  suite.add(BOOST_TEST_CASE(big), 0, valgrind(2));
  suite.add(BOOST_TEST_CASE(many), 0, valgrind(8));
}
