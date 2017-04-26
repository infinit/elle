#include <elle/log.hh>

ELLE_LOG_COMPONENT("elle.protocol.Channel.test");

#include <elle/test.hh>

#include <elle/protocol/ChanneledStream.hh>
#include <elle/protocol/Serializer.hh>
#include <elle/reactor/network/Error.hh>
#include <elle/reactor/network/TCPServer.hh>
#include <elle/reactor/network/TCPSocket.hh>

static
void
_eof(std::function<void (elle::protocol::ChanneledStream&)> server,
     std::function<void (elle::protocol::ChanneledStream&)> client)
{
  elle::reactor::network::TCPServer s;
  s.listen();
  elle::reactor::network::TCPSocket socket(
    elle::reactor::network::TCPSocket::EndPoint(
      boost::asio::ip::address::from_string("127.0.0.1"), s.port()));
  elle::reactor::Thread thread(
    "server",
    [&]
    {
      while (true)
      {
        auto socket = s.accept();
        elle::protocol::Serializer ser(*socket);
        elle::protocol::ChanneledStream channels(ser);
        server(channels);
      }
    });
  elle::protocol::Serializer ser(socket);
  elle::protocol::ChanneledStream channels(ser);
  client(channels);
}

ELLE_TEST_SCHEDULED(eof_accept)
{
  elle::reactor::Barrier accepting;
  _eof(
    [&] (elle::protocol::ChanneledStream& channels)
    {
      elle::reactor::wait(accepting);
    },
    [&] (elle::protocol::ChanneledStream& channels)
    {
      accepting.open();
      BOOST_CHECK_THROW(
        channels.accept(), elle::reactor::network::ConnectionClosed);
    });
}

ELLE_TEST_SCHEDULED(eof_read)
{
  elle::Buffer data("foo");
  elle::reactor::Barrier accepting;
  _eof(
    [&] (elle::protocol::ChanneledStream& channels)
    {
      elle::protocol::Channel c(channels);
      c.write(data);
    },
    [&] (elle::protocol::ChanneledStream& channels)
    {
      std::cerr << "A" << std::endl;
      auto c = channels.accept();
      std::cerr << "B" << std::endl;
      BOOST_TEST(c.read() == data);
      std::cerr << "C" << std::endl;
      BOOST_CHECK_THROW(c.read(), elle::reactor::network::ConnectionClosed);
    });
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  {
    auto eof = BOOST_TEST_SUITE("eof");
    suite.add(eof);
    eof->add(ELLE_TEST_CASE(eof_accept, "accept"), 0, valgrind(1));
    eof->add(ELLE_TEST_CASE(eof_read, "read"), 0, valgrind(1));
  }
}
