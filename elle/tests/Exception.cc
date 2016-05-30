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
#if ! defined INFINIT_WINDOWS && ! defined INFINIT_ANDROID
    BOOST_CHECK_EQUAL(e.backtrace().front().symbol, "thrower()");
#endif
  }
  catch (...)
  {
    BOOST_CHECK(false && "Shouldn't be there");
  }
}

BOOST_AUTO_TEST_CASE(err)
{
  #define CHECK_THROW(statement, Exception, message)                     \
    try                                                                  \
    {                                                                    \
      BOOST_CHECK_THROW(statement, Exception);                           \
      statement;                                                         \
    }                                                                    \
    catch (Exception const& exception)                                   \
    {                                                                    \
      BOOST_CHECK_EQUAL(std::string(exception.what()).find(message), 0); \
    }

  CHECK_THROW(elle::err("foo"), elle::Error, "foo");
  CHECK_THROW(elle::err("foo %s", 3), elle::Error, "foo 3");
  CHECK_THROW(elle::err("%s bar", 3), elle::Error, "3 bar");
  CHECK_THROW(elle::err("%s bar %s", 3, 3), elle::Error, "3 bar 3");
  CHECK_THROW(elle::err("%s", 3, 3), elle::Exception,
    "format error with \"%s\": boost::too_many_args:");
  CHECK_THROW(elle::err("%s %s", 3), elle::Exception,
    "format error with \"%s %s\": boost::too_few_args:");
  CHECK_THROW(elle::err("%s"), elle::Error, "%s");
}
