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
  std::string data(20000, '-');
  // note: there is no garantee utp will buffer that much
  sp.s1->write(data);
  elle::Buffer buf = sp.s2->read(20000);
  ELLE_ASSERT_EQ(buf.size(), 20000);
}

ELLE_TEST_SCHEDULED(many)
{
  SocketPair sp;
  std::string data(1000, '-');
  int iw1 = 0;
  int ir2 = 0;
  reactor::Thread w1("writer 1", [&] {
      for (iw1 =0; iw1<1000; ++iw1)
        sp.s1->reactor::network::UTPSocket::write(elle::ConstWeakBuffer(data));
  });/*
  reactor::Thread w2("writer 2", [&] {
      for (int i=0; i<1000; ++i)
        sp.s2->write(data);
  });
  reactor::Thread r1("reader 1", [&] {
      for (int i=0; i<1000; ++i)
        sp.s2->read(1000);
  });*/
  reactor::Thread r2("reader 2", [&] {
      for (ir2=0; ir2<1000; ++ir2)
        sp.s2->read(1000);
  });
  reactor::Thread watch("watch", [&] {
      while (true)
      {
        reactor::sleep(500_ms);
        std::cerr << "**I: " << iw1 << ' ' << ir2 << std::endl;
      }
  });
  reactor::wait(w1);
  //reactor::wait(w2);
  //reactor::wait(r1);
  reactor::wait(r2);
  sp.s1->stats();
  sp.s2->stats();
  watch.terminate_now();

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


static void loop_socket(reactor::network::UTPSocket* ts)
{
  std::unique_ptr<reactor::network::UTPSocket> s(ts);
  while (true)
  {
    elle::Buffer buf = s->read(4000);
    ELLE_LOG("pong %s", buf.size());
    s->write(buf);
  }
}
static void loop_accept(UTPServer& srv)
{
  while (true)
  {
    auto s = srv.accept();
    auto sptr = s.release();
    new reactor::Thread("socket", [&] { loop_socket(sptr);}, true);
  }
}
static void go(int argc, char** argv)
{
  UTPServer server;
  int port = 0;
  if (argv[1] == std::string("-l"))
  {
    port = std::stoi(argv[2]);
    server.listen(port);
    auto s = server.accept();
    ELLE_LOG("got accept");
    auto& sched = reactor::scheduler();
    std::thread t([&] {
        while (true)
        {
          /*
          std::string line;
          std::getline(std::cin, line);
          if (line.empty())
            return;
          ELLE_LOG("writing");
          sched.mt_run<void>("write", [&] {
              socket.write(elle::ConstWeakBuffer(line));
          });*/

          char buffer[1024];
          std::cin.read(buffer, 1024);
          int sz = std::cin.gcount();
          if (!sz)
            return;
          sched.mt_run<void>("write", [&] {
              s->write(elle::ConstWeakBuffer(buffer, sz));
          });

        }
    });
    while (true)
    {
      elle::Buffer buf = s->read_some(1024);
      ELLE_LOG("got data");
      std::cout.write((const char*)buf.contents(), buf.size());
    }
  }
  else
  {
    server.listen(0);
    std::string host = argv[1];
    int port = std::stoi(argv[2]);
    reactor::network::UTPSocket socket(server);
    socket.connect(host, port);
    ELLE_LOG("connected");
    auto& sched = reactor::scheduler();
    std::thread t([&] {
        while (true)
        {
          /*
          std::string line;
          std::getline(std::cin, line);
          if (line.empty())
            return;
          ELLE_LOG("writing");
          sched.mt_run<void>("write", [&] {
              socket.write(elle::ConstWeakBuffer(line));
          });*/

          char buffer[1024];
          std::cin.read(buffer, 1024);
          int sz = std::cin.gcount();
          if (!sz)
            return;
          sched.mt_run<void>("write", [&] {
              socket.write(elle::ConstWeakBuffer(buffer, sz));
          });

        }
    });
    while (true)
    {
      elle::Buffer buf = socket.read_some(1024);
      ELLE_LOG("got data");
      std::cout.write((const char*)buf.contents(), buf.size());
    }
  }
}

/*
int main(int argc, char** argv)
{
  reactor::Scheduler sched;
  new reactor::Thread(sched, "main", [&] { go(argc, argv);}, true);
  sched.run();
}*/


ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* basics = BOOST_TEST_SUITE("Basics");
  boost::unit_test::framework::master_test_suite().add(basics);
  basics->add(BOOST_TEST_CASE(udp), 0, 5);
  basics->add(BOOST_TEST_CASE(utp_close), 0, 5);
  basics->add(BOOST_TEST_CASE(basic), 0, 5);
  basics->add(BOOST_TEST_CASE(utp_timeout), 0, 5);
  basics->add(BOOST_TEST_CASE(utp_failures), 0, 5);
  basics->add(BOOST_TEST_CASE(streams), 0, 5);
  basics->add(BOOST_TEST_CASE(big), 0, 5);
  basics->add(BOOST_TEST_CASE(many), 0, 20);
}