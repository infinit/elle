#define ELLE_TEST_MODULE Exception
#include <elle/test.hh>
#include <elle/Exception.hh>

// On linux, the function cannot be static
void
thrower();

void
thrower()
{
  throw elle::Exception("test message");
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
