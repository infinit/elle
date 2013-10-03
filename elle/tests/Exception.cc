#define BOOST_TEST_MODULE Exception

#include <boost/test/unit_test.hpp>

#include <elle/Exception.hh>

using elle::Exception;

// Pacify -Wmissing-declarations.
void
thrower();

void
thrower()
{
  throw Exception("test message");
}

BOOST_AUTO_TEST_CASE(ExceptionBacktrace)
{
  try
  {
    thrower();
    BOOST_CHECK(false);
  }
  catch (elle::Exception& e)
  {
    std::cout << e << std::endl;

    BOOST_CHECK_EQUAL(e.what(), "test message");
    BOOST_CHECK_EQUAL(e.backtrace().front().symbol, "thrower()");
  }
}
