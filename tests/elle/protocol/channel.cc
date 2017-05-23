#include <elle/log.hh>

ELLE_LOG_COMPONENT("elle.protocol.Channel.test");

#include <elle/compiler.hh>
#include <elle/test.hh>

#include <elle/protocol/ChanneledStream.hh>
#include <elle/protocol/Serializer.hh>
#include <elle/reactor/network/Error.hh>
#include <elle/reactor/network/TCPServer.hh>
#include <elle/reactor/network/TCPSocket.hh>

template <typename Server, typename Client>
void
_eof(Server server, Client client)
{
  auto s = elle::reactor::network::TCPServer{};
  s.listen();
  auto socket = elle::reactor::network::TCPSocket(
    elle::reactor::network::TCPSocket::EndPoint(
      boost::asio::ip::address::from_string("127.0.0.1"), s.port()));
  auto&& thread ELLE_COMPILER_ATTRIBUTE_MAYBE_UNUSED
    = elle::reactor::Thread("server", [&] {
        while (true)
        {
          auto socket = s.accept();
          auto&& ser = elle::protocol::Serializer(*socket);
          auto&& channels = elle::protocol::ChanneledStream(ser);
          server(channels);
        }
      });
  auto&& ser = elle::protocol::Serializer(socket);
  auto&& channels = elle::protocol::ChanneledStream(ser);
  ELLE_LOG("eof: call client on %s", channels);
  client(channels);
  ELLE_LOG("eof: done");
}

ELLE_TEST_SCHEDULED(eof_accept)
{
  auto accepting = elle::reactor::Barrier{};
  _eof(
    [&] (elle::protocol::ChanneledStream& channels)
    {
      elle::reactor::wait(accepting);
    },
    [&] (elle::protocol::ChanneledStream& channels)
    {
      accepting.open();
      BOOST_CHECK_THROW(channels.accept(),
                        elle::reactor::network::ConnectionClosed);
      BOOST_CHECK_THROW(elle::protocol::Channel{channels},
                        elle::reactor::network::ConnectionClosed);
    });
}

ELLE_TEST_SCHEDULED(eof_read)
{
  auto const data = elle::Buffer("foo");
  auto accepting = elle::reactor::Barrier{};
  _eof(
    [&] (elle::protocol::ChanneledStream& channels)
    {
      ELLE_LOG("server: create channel");
      auto c = elle::protocol::Channel(channels);
      ELLE_LOG("server: created channel: %s, writing data", c);
      c.write(data);
      ELLE_LOG("server: wrote data, quit");
    },
    [&] (elle::protocol::ChanneledStream& channels)
    {
      ELLE_LOG("client: accept");
      auto c = channels.accept();
      ELLE_LOG("client: read");
      BOOST_TEST(c.read() == data);
      ELLE_LOG("client: read again");
      BOOST_CHECK_THROW(c.read(), elle::reactor::network::ConnectionClosed);
      ELLE_LOG("client: quit");
    });
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  {
    auto eof = BOOST_TEST_SUITE("eof");
    suite.add(eof);
    eof->add(ELLE_TEST_CASE(eof_accept, "accept"), 0, valgrind(2));
    eof->add(ELLE_TEST_CASE(eof_read, "read"), 0, valgrind(2));
  }
}
