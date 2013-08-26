#define BOOST_TEST_DYN_LINK

#include <sstream>
#include <thread>

#include <boost/algorithm/string/split.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/test/unit_test.hpp>

#include <elle/finally.hh>
#include <elle/log.hh>
#include <elle/log/Logger.cc>
#include <elle/log/TextLogger.hh>
#include <elle/memory.hh>
#include <elle/os/getenv.hh>
#include <elle/os/setenv.hh>
#include <elle/os/unsetenv.hh>

static
void
plain_indent_test()
{
  elle::log::PlainIndentation indenter;
  indenter.indent();
  indenter.indent();
  BOOST_CHECK_EQUAL(indenter.indentation(), 2);
  indenter.unindent();
  BOOST_CHECK_EQUAL(indenter.indentation(), 1);
}

static
void
message_test()
{
  typedef elle::log::Logger::Level Level;
  elle::os::setenv("ELLE_LOG_LEVEL", "DUMP", 1);

  std::stringstream ss;
  elle::log::TextLogger* logger = new elle::log::TextLogger(ss);
  elle::log::logger(std::unique_ptr<elle::log::Logger>(logger));
  BOOST_CHECK_EQUAL(logger->component_enabled("Test"), Level::dump);

  {
    ELLE_LOG_COMPONENT("Test");
    ELLE_LOG_SCOPE("Test Message");
    BOOST_CHECK_EQUAL(ss.str(), "[1m[Test] Test Message\n[0m");

    ss.str("");
    ELLE_LOG("Another Test Message");
    BOOST_CHECK_EQUAL(ss.str(), "[1m[Test]   Another Test Message\n[0m");

    {
      ELLE_LOG_COMPONENT("Another");

      ss.str("");
      ELLE_LOG("Test");
      BOOST_CHECK_EQUAL(ss.str(), "[1m[Another]   Test\n[0m");

      ss.str("");
      ELLE_TRACE("Test2");
      BOOST_CHECK_EQUAL(ss.str(), "[Another]   Test2\n");

      ss.str("");
      ELLE_DEBUG("Test3");
      BOOST_CHECK_EQUAL(ss.str(), "[Another]   Test3\n");

      ss.str("");
      ELLE_DUMP("Test4");
      BOOST_CHECK_EQUAL(ss.str(), "[Another]   Test4\n");
    }

    ss.str("");
    ELLE_WARN("Test5");
    BOOST_CHECK_EQUAL(ss.str(), "[33;01;33m[ Test  ]   Test5\n[0m");

    ss.str("");
    ELLE_ERR("Test6");
    BOOST_CHECK_EQUAL(ss.str(), "[33;01;31m[ Test  ]   Test6\n[0m");
  }
}

static
void
clear_env()
{
  elle::os::unsetenv("ELLE_LOG_LEVEL", "");
  elle::os::unsetenv("ELLE_LOG_TIME", "");
  elle::os::unsetenv("ELLE_LOG_TIME_UNIVERSAL", "");
  elle::os::unsetenv("ELLE_LOG_PID", "");
}

static
void
environment_format_test()
{
  ELLE_LOG_COMPONENT("Test");

  typedef elle::log::Logger::Level Level;

  std::stringstream ss, res;
  elle::log::TextLogger* logger;

  ss.str("");
  res.str("");
  clear_env();
  elle::os::setenv("ELLE_LOG_TIME", "1", 0);
  BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_TIME"), "1");
  BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_TIME_UNIVERSAL", ""), "");
  BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_PID", ""), "");
  logger = new elle::log::TextLogger(ss);
  elle::log::logger(std::unique_ptr<elle::log::Logger>(logger));
  BOOST_CHECK_EQUAL(logger->component_enabled("Test"),
                    Level::log);

  ELLE_LOG("Test");
  res << "[1m"
      << boost::posix_time::second_clock::local_time()
      << ": [Test] Test\n[0m";
  BOOST_CHECK_EQUAL(ss.str(), res.str());

  ss.str("");
  res.str("");
  clear_env();
  elle::os::setenv("ELLE_LOG_TIME", "1", 0);
  elle::os::setenv("ELLE_LOG_TIME_UNIVERSAL", "1", 0);
  BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_TIME"), "1");
  BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_TIME_UNIVERSAL"), "1");
  BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_PID", ""), "");
  logger = new elle::log::TextLogger(ss);
  elle::log::logger(std::unique_ptr<elle::log::Logger>(logger));
  BOOST_CHECK_EQUAL(logger->component_enabled("Test"),
                    Level::log);

  ELLE_LOG("Test 2");
  res << "[1m"
      << boost::posix_time::second_clock::universal_time() << ": "
      << "[Test] Test 2\n[0m";
  BOOST_CHECK_EQUAL(ss.str(), res.str());

  ss.str("");
  res.str("");
  clear_env();
  elle::os::setenv("ELLE_LOG_PID", "1", 0);
  BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_TIME", ""), "");
  BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_TIME_UNIVERSAL", ""), "");
  BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_PID"), "1");
  logger = new elle::log::TextLogger(ss);
  elle::log::logger(std::unique_ptr<elle::log::Logger>(logger));
  BOOST_CHECK_EQUAL(logger->component_enabled("Test"),
                    Level::log);
  ELLE_LOG("Test 3");
  res << "[1m[" << boost::lexical_cast<std::string>(getpid()) << "] "
      << "[Test] Test 3\n[0m";
  BOOST_CHECK_EQUAL(ss.str(), res.str());

  ss.str("");
  res.str("");
  clear_env();
  elle::os::setenv("ELLE_LOG_TIME", "1", 0);
  elle::os::setenv("ELLE_LOG_TIME_UNIVERSAL", "1", 0);
  elle::os::setenv("ELLE_LOG_PID", "1", 0);
  BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_TIME"), "1");
  BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_TIME_UNIVERSAL"), "1");
  BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_PID"), "1");
  logger = new elle::log::TextLogger(ss);
  elle::log::logger(std::unique_ptr<elle::log::Logger>(logger));
  BOOST_CHECK_EQUAL(logger->component_enabled("Test"),
                    Level::log);
  ELLE_LOG("Test 4");
  res << "[1m"
      << boost::posix_time::second_clock::universal_time() << ": "
      << "[" << boost::lexical_cast<std::string>(getpid()) << "] "
      << "[Test] Test 4\n[0m";
  BOOST_CHECK_EQUAL(ss.str(), res.str());

  ss.str("");
  res.str("");
  clear_env();
  elle::os::setenv("ELLE_LOG_TIME", "1", 0);
  elle::os::setenv("ELLE_LOG_PID", "1", 0);
  BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_TIME"), "1");
  BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_TIME_UNIVERSAL", ""), "");
  BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_PID"), "1");
  logger = new elle::log::TextLogger(ss);
  elle::log::logger(std::unique_ptr<elle::log::Logger>(logger));
  BOOST_CHECK_EQUAL(logger->component_enabled("Test"), Level::log);
  ELLE_WARN("Test 5");
  res << "[33;01;33m"
      << boost::posix_time::second_clock::local_time() << ": "
      << "[" << boost::lexical_cast<std::string>(getpid()) << "] "
      << "[Test] Test 5\n[0m";
  BOOST_CHECK_EQUAL(ss.str(), res.str());

  ss.str("");
  res.str("");
  clear_env();
  elle::os::setenv("ELLE_LOG_DISPLAY_TYPE", "1", 1);
  BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_DISPLAY_TYPE"), "1");
  logger = new elle::log::TextLogger(ss);
  elle::log::logger(std::unique_ptr<elle::log::Logger>(logger));
  BOOST_CHECK_EQUAL(logger->component_enabled("Test"), Level::log);
  ELLE_WARN("Test 5");
  res << "[33;01;33m"
      << "[Test] [warning] Test 5\n[0m";
  BOOST_CHECK_EQUAL(ss.str(), res.str());
  ELLE_ERR("Test 6")
  res << "[33;01;33m"
      << "[Test] [error] Test 6\n[0m";
  elle::os::setenv("ELLE_LOG_DISPLAY_TYPE", "0", 1);

  elle::log::logger(std::unique_ptr<elle::log::Logger>(nullptr));
}

static
void
parallel_write()
{
  std::stringstream output;
  elle::log::TextLogger logger(output);
  logger.component_enabled("in");
  logger.component_enabled("out");

  auto action = [&logger](int& counter)
    {
      using namespace boost::posix_time;
      ptime deadline = microsec_clock::local_time() + seconds(1);
      while (microsec_clock::local_time() < deadline)
      {
        ELLE_LOG_COMPONENT("out");
        ELLE_LOG("out")
        {
          ELLE_LOG_COMPONENT("in");
          ELLE_ERR("in");
        }
        ++counter;
      }
    };

  int c1 = 0;;
  int c2 = 0;
  {
    std::thread t1([&](){ action(c1); });
    std::thread t2([&](){ action(c2); });

    elle::Finally join([&]
                       {
                         t1.join();
                         t2.join();
                       });
  }

  BOOST_CHECK_GT(c1, 64);
  BOOST_CHECK_GT(c2, 64);
}

static
void
multiline()
{
  std::stringstream output;
  elle::log::logger(
    std::unique_ptr<elle::log::Logger>{new elle::log::TextLogger{output}});
  ELLE_LOG_COMPONENT("multiline");
  ELLE_LOG("This message\nis\nsplitted\n\ninto\r\n5 lines\n\n\r\n\r\r");

  std::vector<std::string> lines;
  auto str = output.str();
  boost::split(lines,
               str,
               boost::algorithm::is_any_of("\r\n"),
               boost::token_compress_on);
  BOOST_CHECK_EQUAL(lines.size(), 6);
}

#ifndef BOOST_CHECK_NOT_EQUAL
template<typename T>
static
void
BOOST_CHECK_NOT_EQUAL(T a, T b)
{
  BOOST_CHECK_PREDICATE(std::not_equal_to<T>(), (a)(b));
}
#endif

static
void
trim()
{
  std::stringstream output;
  // XXX This does not work for the moment.
  // elle::os::setenv("ELLE_LOG_COLOR", "0");
  elle::log::logger(
    std::unique_ptr<elle::log::Logger>{new elle::log::TextLogger{output}});
  ELLE_LOG_COMPONENT("trim");
  ELLE_LOG("   \n\t\t\tThis message is trimmed !    \n\n\r\n\r\r\t ");
  std::string out = output.str();
  {
    std::cerr << "OUTPUT: >>>" << out << "<<<\n";
    unsigned int pos = 0;
    for (unsigned int pos = 0; pos < out.size() - 1; ++pos)
    {
      std::cerr << pos << ' ' << out.size() << std::endl;
      BOOST_CHECK_NOT_EQUAL(out[pos], '\t');
      // XXX When colors can be disabled, these tests should pass.
      //BOOST_CHECK_NOT_EQUAL(out[pos], '\r');
      //BOOST_CHECK_NOT_EQUAL(out[pos], '\n');
    }
  }
}

static
bool
test_suite()
{
  boost::unit_test::test_suite* indentation = BOOST_TEST_SUITE("Indentation");
  boost::unit_test::framework::master_test_suite().add(indentation);
  indentation->add(BOOST_TEST_CASE(std::bind(plain_indent_test)));

  boost::unit_test::test_suite* logger = BOOST_TEST_SUITE("Logger");
  boost::unit_test::framework::master_test_suite().add(logger);
  logger->add(BOOST_TEST_CASE(message_test));
  logger->add(BOOST_TEST_CASE(environment_format_test));

  boost::unit_test::test_suite* concurrency = BOOST_TEST_SUITE("Concurrency");
  boost::unit_test::framework::master_test_suite().add(concurrency);
  concurrency->add(BOOST_TEST_CASE(std::bind(parallel_write)));

  boost::unit_test::test_suite* format = BOOST_TEST_SUITE("Format");
  boost::unit_test::framework::master_test_suite().add(format);
  concurrency->add(BOOST_TEST_CASE(std::bind(multiline)));
  concurrency->add(BOOST_TEST_CASE(std::bind(trim)));
  return true;
}

int
main(int argc, char** argv)
{
  elle::os::setenv("ELLE_LOG_COLOR", "1", 0);
  return ::boost::unit_test::unit_test_main(test_suite, argc, argv);
}
