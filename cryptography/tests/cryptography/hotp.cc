#include <cryptography/hotp.hh>
# include <elle/test.hh>


using namespace infinit::cryptography;
static void test_basic()
{
  HOTPServer server;
  HOTPClient client(server.key());
  for (int i=0; i< 1000; ++i)
  {
    BOOST_CHECK_EQUAL(server.validate(client.next()), HOTPServer::ValidationResult::ok);
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