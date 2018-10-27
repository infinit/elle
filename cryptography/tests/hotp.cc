#include <elle/cryptography/hotp.hh>
# include <elle/test.hh>
# include <elle/log.hh>

ELLE_LOG_COMPONENT("test");

using namespace elle::cryptography;
static void test_basic()
{
  HOTPServer server;
  HOTPServer server2;
  BOOST_CHECK(server.key() != server2.key());
  HOTPClient client(server.key());
  for (int i=0; i< 1000; ++i)
  {
    int32_t cn = client.next();
    ELLE_DUMP("next: %s", cn);
    BOOST_CHECK_EQUAL(server.validate(cn), HOTPServer::ValidationResult::ok);
  }
  int32_t v1 = client.next();
  int32_t v2 = client.next();
  int32_t v3 = client.next();
  BOOST_CHECK_EQUAL(server.validate(v2), HOTPServer::ValidationResult::ok);
  BOOST_CHECK_EQUAL(server.validate(v1), HOTPServer::ValidationResult::ok);
  BOOST_CHECK_EQUAL(server.validate(v3), HOTPServer::ValidationResult::ok);
  BOOST_CHECK_EQUAL(server.validate(v2), HOTPServer::ValidationResult::replay);
  BOOST_CHECK_EQUAL(server.validate(12), HOTPServer::ValidationResult::not_found);
}

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("hotp");
  suite->add(BOOST_TEST_CASE(test_basic));
  boost::unit_test::framework::master_test_suite().add(suite);
}
