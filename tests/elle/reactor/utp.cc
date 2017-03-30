#include <elle/assert.hh>
#include <elle/test.hh>

#include <elle/reactor/network/Error.hh>
#include <elle/reactor/network/udp-socket.hh>
#include <elle/reactor/network/utp-server.hh>
#include <elle/reactor/network/utp-socket.hh>
#include <elle/reactor/scheduler.hh>

ELLE_LOG_COMPONENT("utpcat");

using namespace elle::reactor::network;

ELLE_TEST_SCHEDULED(udp)
{
  elle::reactor::network::UDPSocket s;
  s.socket()->close();
  s.bind(boost::asio::ip::udp::endpoint({}, 0));
  elle::reactor::Thread t("recv", [&] {
      char data[1000];
      boost::asio::ip::udp::endpoint ep;
      s.receive_from(elle::WeakBuffer(data), ep);
    });
  elle::reactor::sleep(100_ms);
  t.terminate();
  elle::reactor::wait(t);
}


class SocketPair
{
public:
  SocketPair();
  elle::reactor::network::UTPServer srv1, srv2;
  std::unique_ptr<elle::reactor::network::UTPSocket> s1, s2;
};

ELLE_TEST_SCHEDULED(not_connected)
{
  UTPServer s;
  UTPSocket sock(s);
  BOOST_CHECK_THROW(sock.read(1), elle::reactor::network::Error);
}

ELLE_TEST_SCHEDULED(basic)
{
  {
    SocketPair sp;
    sp.s1->write("foo");
    ELLE_ASSERT_EQ(sp.s2->read_some(3).string(), "foo");
    sp.s2->write("bar");
    ELLE_ASSERT_EQ(sp.s1->read_some(3).string(), "bar");
    sp.s1->write("baz");
    sp.s2->write("bam");
    ELLE_ASSERT_EQ(sp.s1->read_some(3).string(), "bam");
    ELLE_ASSERT_EQ(sp.s2->read_some(3).string(), "baz");
    ELLE_LOG("done");
  }
  ELLE_LOG("done2");
}

ELLE_TEST_SCHEDULED(utp_close)
{
  SocketPair sp;
  sp.s1->write("foo");
  sp.s1->close();
  elle::reactor::sleep(100_ms);
  BOOST_CHECK_THROW(sp.s1->write("bar"),  elle::reactor::network::ConnectionClosed);
  BOOST_CHECK_THROW(sp.s1->read_some(10), elle::reactor::network::ConnectionClosed);
  BOOST_CHECK_THROW(sp.s2->read_some(10), elle::reactor::network::ConnectionClosed);
  BOOST_CHECK_THROW(sp.s2->write("bar"),  elle::reactor::network::ConnectionClosed);
  {
    UTPServer srv;
    elle::reactor::network::UTPSocket sock(srv);
  }
  {
    SocketPair sp;
  }
  {
    UTPServer srv1, srv2;
    elle::reactor::Thread* t;
    srv1.listen(0); srv2.listen(0);
    {
      auto s1 = std::make_unique<elle::reactor::network::UTPSocket>(srv2);
      elle::reactor::Barrier b;
      t = new elle::reactor::Thread ("foo", [&] {
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
  char* megabuf;
  {
  SocketPair sp;
  BOOST_CHECK_THROW(sp.s1->read(10, 100_ms), elle::reactor::network::TimeOut);
  sp.s2->write("foooo");
  BOOST_CHECK_THROW(sp.s1->read(10, 100_ms), elle::reactor::network::TimeOut);
  megabuf = (char*)malloc(10000000);
  memset(megabuf, 0, 10000000);
  BOOST_CHECK_THROW(sp.s1->write(elle::ConstWeakBuffer(megabuf, 10000000),
                                 100_ms),
                    elle::reactor::network::TimeOut);
  }
  free(megabuf);
}

#ifdef INFINIT_LINUX
ELLE_TEST_SCHEDULED(utp_failures)
{
  SocketPair sp;
  elle::reactor::network::UTPSocket socket(sp.srv1);
  ELLE_LOG("connect");
  BOOST_CHECK_THROW(socket.connect("127.0.0.1", 65530), ConnectionRefused);
  // check other sockets are unaffected
  sp.s1->write("foo");
  ELLE_LOG("read");
  ELLE_ASSERT_EQ(sp.s2->read(3), "foo");
}
#endif

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
  auto data = std::string(buffer_size, '-');
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
  elle::reactor::Thread::unique_ptr w1(
    new elle::reactor::Thread("writer 1", [&] {
        for (iw1 = 0; iw1 < iterations; ++iw1)
          sp.s1->elle::reactor::network::UTPSocket::write(elle::ConstWeakBuffer(data));
      }));
  elle::reactor::Thread::unique_ptr r2(
    new elle::reactor::Thread("reader 2", [&] {
        for (ir2 = 0; ir2 < iterations; ++ir2)
          sp.s2->read(1000);
      }));
  elle::reactor::Thread::unique_ptr watch(
    new elle::reactor::Thread("watch", [&] {
        while (true)
        {
          elle::reactor::sleep(500_ms);
          std::cerr << "**I: " << iw1 << ' ' << ir2 << std::endl;
        }
      }));
  elle::reactor::wait(*w1);
  elle::reactor::wait(*r2);
  sp.s1->stats();
  sp.s2->stats();
}

ELLE_TEST_SCHEDULED(destruction)
{
  for (int y=0; y<20; ++y)
  {
    SocketPair sp;
    sp.s1->write("foo");
    ELLE_ASSERT_EQ(sp.s2->read_some(3).string(), "foo");
    sp.s2->write("bar");
    ELLE_ASSERT_EQ(sp.s1->read_some(3).string(), "bar");
    sp.s1->write("baz");
    sp.s2->write("bam");
    ELLE_ASSERT_EQ(sp.s1->read_some(3).string(), "bam");
    ELLE_ASSERT_EQ(sp.s2->read_some(3).string(), "baz");
    ELLE_LOG("done");
    sp.s1.reset();
    sp.s2.reset();
    for (int i=0; i<y; ++i)
      elle::reactor::yield();
  }
}

SocketPair::SocketPair()
{
  srv1.listen(0);
  srv2.listen(0);
  s1 = std::make_unique<elle::reactor::network::UTPSocket>(srv2);
  s1->connect("127.0.0.1",
              srv1.local_endpoint().port());
  ELLE_LOG("SocketPair accepting");
  s2 = srv1.accept();
  ELLE_LOG("SocketPair ready");
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(not_connected), 0, valgrind(2));
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
  suite.add(BOOST_TEST_CASE(destruction), 0, valgrind(2));
}
