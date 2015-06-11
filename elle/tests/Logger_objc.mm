#include <elle/test.hh>

#include <elle/log.hh>
#include <elle/log/TextLogger.hh>
#include <elle/os/environ.hh>

#include <Foundation/Foundation.h>

static
void
objc_types()
{
  typedef elle::log::Logger::Level Level;

  std::stringstream ss;
  elle::log::TextLogger* logger = new elle::log::TextLogger(ss, "DUMP", true);
  elle::log::logger(std::unique_ptr<elle::log::Logger>(logger));
  BOOST_CHECK_EQUAL(logger->component_enabled("Test"), Level::dump);
  {
    ELLE_LOG_COMPONENT("Test");
    ELLE_LOG_SCOPE("%s", @"Test Message");
    BOOST_CHECK_EQUAL(ss.str(), "[1m[Test] Test Message\n[0m");

    ss.str("");
    NSString* s_uninitialised;
    ELLE_LOG("%s", s_uninitialised);
    BOOST_CHECK_EQUAL(ss.str(), "[1m[Test]   <nil>\n[0m");

    ss.str("");
    NSNumber* number = @42;
    ELLE_LOG("%s", number);
    BOOST_CHECK_EQUAL(ss.str(), "[1m[Test]   42\n[0m");

    ss.str("");
    NSNumber* number_nil = nil;
    ELLE_LOG("%s", number_nil);
    BOOST_CHECK_EQUAL(ss.str(), "[1m[Test]   <nil>\n[0m");

    ss.str("");
    ELLE_LOG("%s %s", @"Test Message", number);
    BOOST_CHECK_EQUAL(ss.str(), "[1m[Test]   Test Message 42\n[0m");

    ss.str("");
    NSString* s_formatted = [NSString stringWithFormat:@"Bob %@", number];
    ELLE_LOG("%s", s_formatted);
    BOOST_CHECK_EQUAL(ss.str(), "[1m[Test]   Bob 42\n[0m");
  }
}


ELLE_TEST_SUITE()
{
  elle::os::setenv("ELLE_LOG_COLOR", "1", 0);
  boost::unit_test::test_suite* logger = BOOST_TEST_SUITE("Logger.ObjC");
  boost::unit_test::framework::master_test_suite().add(logger);
  logger->add(BOOST_TEST_CASE(objc_types));
}
