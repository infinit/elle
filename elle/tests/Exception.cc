#define BOOST_TEST_MODULE Exception

#include <boost/test/unit_test.hpp>

#include <elle/Exception.hh>

using elle::Exception;

static
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
    BOOST_CHECK(false && "Shouldn't be there");
  }
  catch (elle::Exception& e)
  {
    BOOST_CHECK_EQUAL(e.what(), "test message");
#ifndef INFINIT_WINDOWS
    BOOST_CHECK_EQUAL(e.backtrace().front().symbol, "thrower()");
#endif
  }
  catch (...)
  {
    BOOST_CHECK(false && "Shouldn't be there");
  }
}
