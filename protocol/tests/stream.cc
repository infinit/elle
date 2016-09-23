#include <protocol/exceptions.hh>
#include <protocol/Serializer.hh>
#include <protocol/ChanneledStream.hh>
#include <protocol/Channel.hh>

#include <cryptography/random.hh>

#include <reactor/asio.hh>
#include <reactor/Barrier.hh>
#include <reactor/Scope.hh>
#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/tcp-server.hh>
#include <reactor/network/tcp-socket.hh>
#include <reactor/scheduler.hh>

#include <elle/test.hh>
#include <elle/cast.hh>
#include <elle/IOStream.hh>
#include <elle/With.hh>

ELLE_LOG_COMPONENT("infinit.protocol.test");

// struct Stream
//   : public infinit::protocol::Stream
// {
//   elle::Buffer
//   read() override
//   {
//     return {};
//   }

//   void
//   _write(elle::Buffer const& packet) override
//   {
//   }
// };

ELLE_TEST_SCHEDULED(buffer)
{
  elle::Buffer x;
  infinit::protocol::Stream::uint32_put(x, std::numeric_limits<uint32_t>::min());
  infinit::protocol::Stream::uint32_put(x, std::numeric_limits<uint32_t>::max());
  infinit::protocol::Stream::uint32_put(x, std::numeric_limits<uint32_t>::min());
  infinit::protocol::Stream::uint32_put(x, 3);
  BOOST_CHECK_EQUAL(infinit::protocol::Stream::uint32_get(x), std::numeric_limits<uint32_t>::min());
  BOOST_CHECK_EQUAL(infinit::protocol::Stream::uint32_get(x), std::numeric_limits<uint32_t>::max());
  BOOST_CHECK_EQUAL(infinit::protocol::Stream::uint32_get(x), std::numeric_limits<uint32_t>::min());
  BOOST_CHECK_EQUAL(infinit::protocol::Stream::uint32_get(x), 3);
  BOOST_CHECK_EQUAL(x.size(), 0);
}

ELLE_TEST_SCHEDULED(stream)
{
  elle::Buffer x;
  {
    elle::IOStream o(x.ostreambuf());
    infinit::protocol::Stream::uint32_put(o, std::numeric_limits<uint32_t>::min());
    infinit::protocol::Stream::uint32_put(o, std::numeric_limits<uint32_t>::max());
    infinit::protocol::Stream::uint32_put(o, std::numeric_limits<uint32_t>::min());
    infinit::protocol::Stream::uint32_put(o, 3);
  }
  // XXX.
  {
    elle::IOStream i(x.istreambuf());
    BOOST_CHECK_EQUAL(infinit::protocol::Stream::uint32_get(i), std::numeric_limits<uint32_t>::min());
    BOOST_CHECK_EQUAL(infinit::protocol::Stream::uint32_get(i), std::numeric_limits<uint32_t>::max());
    BOOST_CHECK_EQUAL(infinit::protocol::Stream::uint32_get(i), std::numeric_limits<uint32_t>::min());
    BOOST_CHECK_EQUAL(infinit::protocol::Stream::uint32_get(i), 3);
  }
  // XXX.
  {
    {
      elle::IOStream i(x.istreambuf());
      BOOST_CHECK_EQUAL(infinit::protocol::Stream::uint32_get(i), std::numeric_limits<uint32_t>::min());
      BOOST_CHECK_EQUAL(infinit::protocol::Stream::uint32_get(i), std::numeric_limits<uint32_t>::max());
    }
    {
      elle::IOStream i(x.istreambuf());
      BOOST_CHECK_EQUAL(infinit::protocol::Stream::uint32_get(i), std::numeric_limits<uint32_t>::min());
      BOOST_CHECK_EQUAL(infinit::protocol::Stream::uint32_get(i), std::numeric_limits<uint32_t>::max());
    }
  }
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(buffer), 0, valgrind(6, 10));
  suite.add(BOOST_TEST_CASE(stream), 0, valgrind(6, 10));
}
