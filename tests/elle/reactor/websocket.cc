#include <elle/reactor/http/WebSocket.hh>
#include <elle/test.hh>

// This test is not standalone and relies on an external global server.
ELLE_TEST_SCHEDULED(echo, (bool, ssl))
{
  elle::reactor::http::WebSocket sock(
    elle::print("{}://demos.kaazing.com/echo", ssl ? "wss" : "ws"));
  elle::Buffer data(
    "WebSocket is a computer communications protocol, providing full-duplex "
    "communication channels over a single TCP connection. The WebSocket "
    "protocol was standardized by the IETF as RFC 6455 in 2011, and the "
    "WebSocket API in Web IDL is being standardized by the W3C.");
  sock.send(data);
  auto received = sock.receive();
  BOOST_CHECK_EQUAL(received, data);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(
    ELLE_TEST_CASE(std::bind(echo, true), "echo_ssl"), 0, valgrind(30));
  suite.add(
    ELLE_TEST_CASE(std::bind(echo, false), "echo_plain"), 0, valgrind(30));
}
