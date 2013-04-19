#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <elle/Exception.hh>

using elle::Exception;

void
thrower()
{
  throw Exception("test message");
}

static
void
test_exception()
{
  try
  {
    thrower();
    BOOST_CHECK(false);
  }
  catch (elle::Exception& e)
  {
    BOOST_CHECK_EQUAL(e.what(), "test message");
    BOOST_CHECK_EQUAL(e.backtrace().front().symbol, "thrower()");
  }
}

static
bool
test_suite()
{
  boost::unit_test::test_suite* exn = BOOST_TEST_SUITE("Exception");
  boost::unit_test::framework::master_test_suite().add(exn);
  exn->add(BOOST_TEST_CASE(test_exception));

  return true;
}

int
main(int argc, char** argv)
{
  return ::boost::unit_test::unit_test_main(test_suite, argc, argv);
}
