#include <regex>
#include <sstream>
#include <thread>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp> // boost::contains
#include <boost/algorithm/string/trim.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>

#include <elle/test.hh>

#include <elle/bytes.hh> // _KiB
#include <elle/filesystem/TemporaryDirectory.hh>
#include <elle/finally.hh>
#include <elle/fstream.hh>
#include <elle/log.hh>
#include <elle/log/CompositeLogger.hh>
#include <elle/log/FileLogger.hh>
#include <elle/log/Logger.hh>
#include <elle/log/SysLogger.hh>
#include <elle/log/TextLogger.hh>
#include <elle/memory.hh>
#include <elle/os/environ.hh>

using namespace std::literals;
namespace fs = std::filesystem;

ELLE_LOG_COMPONENT("elle.log");

/*---------------------.
| test suite: logger.  |
`---------------------*/

namespace
{
  void
  _message_test(bool env)
  {
    using Level = elle::log::Logger::Level;

    std::stringstream ss;
    auto logger = std::unique_ptr<elle::log::Logger>{};
    if (env)
    {
      elle::os::setenv("ELLE_LOG_LEVEL", "DUMP");
      elle::os::setenv("ELLE_LOG_DISPLAY_TYPE", "1");
      logger = std::make_unique<elle::log::TextLogger>(ss);
    }
    else
      logger = std::make_unique<elle::log::TextLogger>(ss, "DUMP", "ELLE_LOG_LEVEL",
                                                       true);
    BOOST_TEST(logger->component_level("Test") == Level::dump);

    auto prev = elle::log::logger(std::move(logger));

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

        ss.str("");
        ELLE_DUMP("Test5")
        {
          BOOST_CHECK_EQUAL(ss.str(), "[Another]   Test5\n");
          ss.str("");
          ELLE_DUMP("Test5.1")
          {
            BOOST_CHECK_EQUAL(ss.str(), "[Another]     Test5.1\n");
            ss.str("");
            ELLE_DUMP("Test5.1.1");
            BOOST_CHECK_EQUAL(ss.str(), "[Another]       Test5.1.1\n");
          }
        }
      }

      ss.str("");
      ELLE_WARN("Test5");
      BOOST_CHECK_EQUAL(ss.str(), "[33;01;33m[ Test  ] [warning]   Test5\n[0m");

      ss.str("");
      ELLE_ERR("Test6");
      BOOST_CHECK_EQUAL(ss.str(), "[33;01;31m[ Test  ] [error]   Test6\n[0m");
    }
    elle::log::logger(std::move(prev));
  }

  void
  message_test()
  {
    _message_test(false);
    _message_test(true);
  }

  void
  clear_env()
  {
    elle::os::unsetenv("ELLE_LOG_LEVEL");
    elle::os::unsetenv("ELLE_LOG_TIME");
    elle::os::unsetenv("ELLE_LOG_TIME_UNIVERSAL");
    elle::os::unsetenv("ELLE_LOG_PID");
  }

  /// Decompose ("parse") a log message.
  struct LogMessage
  {
    LogMessage(std::string const& line)
    {
      BOOST_TEST_MESSAGE("msg: {" << line << "}");
      auto const re =
        std::regex{"(?:"
                   "" "(.*?-.*?-.*?)"                       // 1: date.
                   "" " "
                   "" "([0-9][0-9]:[0-9][0-9]:[0-9][0-9])"  // 2: time in secs.
                   "" "(?:\\.([0-9]{6}))?"                  // 3: microseconds.
                   "" ": "
                   ")?"
                   "(?:"
                   "" "\\[(.*?)\\]"                         // 4: component.
                   ")?"
                   " "
                   "(.*)"};                                 // 5: message.
      auto m = std::smatch{};
      BOOST_REQUIRE(std::regex_match(line, m, re));
      ELLE_LOG("date: {}, time: {}, microsec: {}, component: {}, message: {}",
               m[1], m[2], m[3], m[4], m[5]);
      if (m[1].length())
        this->date = m[1];
      if (m[2].length())
        this->time = m[2];
      if (m[3].length())
        this->microsec = m[3];
      if (m[4].length())
        this->component = m[4];
      this->message = m[5];
    }
    std::string date;
    std::string time;
    std::string microsec;
    std::string component;
    std::string message;
  };
}

static
void
_environment_format_test(bool env)
{
  ELLE_LOG_COMPONENT("Test");

  using Level = elle::log::Logger::Level;

  std::stringstream ss, res;
  elle::log::TextLogger* logger;

  ss.str("");
  res.str("");
  clear_env();
  if (env)
  {
    // FIXME: Checking time printing is non-deterministic.
    // elle::os::setenv("ELLE_LOG_TIME", "1", 0);
    // BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_TIME"), "1");
    BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_TIME_UNIVERSAL", ""), "");
    BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_PID", ""), "");
    logger = new elle::log::TextLogger(ss);
  }
  else
  {
    logger = new elle::log::TextLogger(
      ss,
      "",     // log_level
      "ELLE_LOG_LEVEL", // envvar
      false,  // display_type
      false,  // enable_pid
      false,  // enable_tid
      false, // true, enable_time. FIXME: Checking time printing is non-deterministic.
      false   // universal_time
    );
  }
  auto prev = elle::log::logger(std::unique_ptr<elle::log::Logger>(logger));
  BOOST_CHECK_EQUAL(logger->component_level("Test"),
                    Level::log);
  ELLE_LOG("Test");
  // FIXME: Checking time printing is non-deterministic.
  // auto time = boost::posix_time::second_clock::local_time();
  // res << "[1m" << time << ": [Test] Test\n[0m";
  res << "[1m[Test] Test\n[0m";
  BOOST_CHECK_EQUAL(ss.str(), res.str());

  ss.str("");
  res.str("");
  clear_env();
  if (env)
  {
    // FIXME: Checking time printing is non-deterministic.
    // elle::os::setenv("ELLE_LOG_TIME", "1", 0);
    // elle::os::setenv("ELLE_LOG_TIME_UNIVERSAL", "1", 0);
    // BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_TIME"), "1");
    // BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_TIME_UNIVERSAL"), "1");
    BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_PID", ""), "");
    logger = new elle::log::TextLogger(ss);
  }
  else
  {
    logger = new elle::log::TextLogger(
      ss,
      "",     // log_level
      "ELLE_LOG_LEVEL", // envvar
      false,  // display_type
      false,  // enable_pid
      false,  // enable_tid
      // FIXME: Checking time printing is non-deterministic.
      // true,   // enable_time
      false,
      true    // universal_time
    );
  }
  elle::log::logger(std::unique_ptr<elle::log::Logger>(logger));
  BOOST_CHECK_EQUAL(logger->component_level("Test"),
                    Level::log);

  ELLE_LOG("Test 2");
  // FIXME: Checking time printing is non-deterministic.
  // res << "[1m"
  //     << Clock::now() << ": "
  //     << "[Test] Test 2\n[0m";
  res << "[1m[Test] Test 2\n[0m";
  BOOST_CHECK_EQUAL(ss.str(), res.str());

  ss.str("");
  res.str("");
  clear_env();
  if (env)
  {
    elle::os::setenv("ELLE_LOG_PID", "1", 0);
    BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_TIME", ""), "");
    BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_TIME_UNIVERSAL", ""), "");
    BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_PID"), "1");
    logger = new elle::log::TextLogger(ss);
  }
  else
  {
    logger = new elle::log::TextLogger(
      ss,
      "",     // log_level
      "ELLE_LOG_LEVEL", // envvar
      false,  // display_type
      true,   // enable_pid
      false,  // enable_tid
      false,  // enable_time
      false   // universal_time
    );
  }
  elle::log::logger(std::unique_ptr<elle::log::Logger>(logger));
  BOOST_CHECK_EQUAL(logger->component_level("Test"),
                    Level::log);
  ELLE_LOG("Test 3");
  res << "[1m[Test] [" << boost::lexical_cast<std::string>(getpid()) << "] "
      << "Test 3\n[0m";
  BOOST_CHECK_EQUAL(ss.str(), res.str());

  ss.str("");
  res.str("");
  clear_env();
  if (env)
  {
    // FIXME: Checking time printing is non-deterministic.
    // elle::os::setenv("ELLE_LOG_TIME", "1", 0);
    // elle::os::setenv("ELLE_LOG_TIME_UNIVERSAL", "1", 0);
    // BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_TIME"), "1");
    // BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_TIME_UNIVERSAL"), "1");
    elle::os::setenv("ELLE_LOG_PID", "1", 0);
    BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_PID"), "1");
    logger = new elle::log::TextLogger(ss);
  }
  else
  {
    logger = new elle::log::TextLogger(
      ss,
      "",     // log_level
      "ELLE_LOG_LEVEL", // envvar
      false,  // display_type
      true,   // enable_pid
      false,  // enable_tid
      // FIXME: Checking time printing is non-deterministic.
      // true,   // enable_time
      // true    // universal_time
      false,
      false
    );
  }
  elle::log::logger(std::unique_ptr<elle::log::Logger>(logger));
  BOOST_CHECK_EQUAL(logger->component_level("Test"),
                    Level::log);
  ELLE_LOG("Test 4");
  // FIXME: Checking time printing is non-deterministic.
  // res << "[1m"
  //     << Clock::now() << ": "
  //     << "[Test] [" << boost::lexical_cast<std::string>(getpid()) << "] "
  //     << "Test 4\n[0m";
  res << "[1m[Test] [" << boost::lexical_cast<std::string>(getpid()) << "] "
      << "Test 4\n[0m";
  BOOST_CHECK_EQUAL(ss.str(), res.str());

  ss.str("");
  res.str("");
  clear_env();
  if (env)
  {
    // FIXME: Checking time printing is non-deterministic.
    // elle::os::setenv("ELLE_LOG_TIME", "1", 0);
    // elle::os::setenv("ELLE_LOG_PID", "1", 0);
    // BOOST_TEST(elle::os::getenv("ELLE_LOG_TIME") == "1");
    // BOOST_TEST(elle::os::getenv("ELLE_LOG_TIME_UNIVERSAL", "") == "");
    BOOST_CHECK_EQUAL(elle::os::getenv("ELLE_LOG_PID"), "1");
    logger = new elle::log::TextLogger(ss);
  }
  else
  {
    logger = new elle::log::TextLogger(
      ss,
      "",     // log_level
      "ELLE_LOG_LEVEL", // envvar
      false,  // display_type
      true,   // enable_pid
      false,  // enable_tid
      // FIXME: Checking time printing is non-deterministic.
      // true,   // enable_time
      false,
      false   // universal_time
    );
  }
  elle::log::logger(std::unique_ptr<elle::log::Logger>(logger));
  BOOST_CHECK_EQUAL(logger->component_level("Test"), Level::log);
  ELLE_WARN("Test 5");
  // FIXME: Checking time printing is non-deterministic.
  // res << "[33;01;33m"
  //     << boost::posix_time::second_clock::local_time() << ": "
  //     << "[Test] [" << boost::lexical_cast<std::string>(getpid()) << "] "
  //     << "[warning] Test 5\n[0m";
  res << "[33;01;33m[Test] ["
      << boost::lexical_cast<std::string>(getpid()) << "] "
      << "[warning] Test 5\n[0m";
  BOOST_CHECK_EQUAL(ss.str(), res.str());

  ss.str("");
  res.str("");
  clear_env();
  if (env)
  {
    elle::os::setenv("ELLE_LOG_DISPLAY_TYPE", "1");
    BOOST_TEST(elle::os::getenv("ELLE_LOG_DISPLAY_TYPE") == "1");
    logger = new elle::log::TextLogger(ss);
  }
  else
  {
    logger = new elle::log::TextLogger(
      ss,
      "",     // log_level
      "ELLE_LOG_LEVEL", // envvar
      true,   // display_type
      false,  // enable_pid
      false,  // enable_tid
      false,  // enable_time
      false   // universal_time
    );
  }
  elle::log::logger(std::unique_ptr<elle::log::Logger>(logger));
  BOOST_CHECK_EQUAL(logger->component_level("Test"), Level::log);
  ELLE_WARN("Test 5");
  res << "[33;01;33m"
      << "[Test] [warning] Test 5\n[0m";
  BOOST_CHECK_EQUAL(ss.str(), res.str());
  ELLE_ERR("Test 6")
  res << "[33;01;33m"
      << "[Test] [error] Test 6\n[0m";
  elle::os::setenv("ELLE_LOG_DISPLAY_TYPE", "0");

  elle::log::logger(std::move(prev));
}

namespace
{
  void
  environment_format_test()
  {
    _environment_format_test(false);
    // _environment_format_test(true);
  }

  /// Check CompositeLogger.
  void
  composite()
  {
    auto logger = std::make_unique<elle::log::CompositeLogger>();
    auto& loggers = logger->loggers();
    auto&& log1 = std::stringstream{};
    loggers.push_back(std::make_unique<elle::log::TextLogger>(log1));
    loggers.back()->log_level("log1:DUMP");

    auto&& log2 = std::stringstream{};
    loggers.push_back(std::make_unique<elle::log::TextLogger>(log2));
    loggers.back()->log_level("log2:TRACE");

    auto prev = elle::log::logger(std::move(logger));
    {
      ELLE_LOG_COMPONENT("log1");
      ELLE_DUMP("Donald Dump");
    }
    {
      ELLE_LOG_COMPONENT("log2");
      ELLE_TRACE("Samo Trace");
    }
    BOOST_TEST(log1.str() == "[log1] Donald Dump\n");
    BOOST_TEST(log2.str() == "[log2] Samo Trace\n");
    elle::log::logger(std::move(prev));
  }

  /*-------------------.
  | Check FileLogger.  |
  `-------------------*/
  void
  file()
  {
    auto d = elle::filesystem::TemporaryDirectory{};
    auto const base = d.path() / "file.log";

    for (auto phase: {1, 2})
    {
      // a. Generate the logs.
      if (phase == 1)
      {
        auto logger = std::make_unique<elle::log::FileLogger>(base, "DUMP", 512, 3);
        BOOST_CHECK_EQUAL(logger->base(), base);
        auto prev = elle::log::logger(std::move(logger));
        {
          ELLE_LOG_COMPONENT("log");
          for (int i = 0; i < 100; ++i)
          {
            ELLE_DUMP("ping");
            ELLE_DUMP("pong");
          }
        }
        elle::log::logger(std::move(prev));
      }
      else if (phase == 2)
      {
        // Check append by adding "append" to the last log, and
        // checking there is no new file.
        auto logger =
          std::make_unique<elle::log::FileLogger>(base, "DUMP", 512, 3, true);
        auto prev = elle::log::logger(std::move(logger));
        {
          ELLE_LOG_COMPONENT("log");
          ELLE_DUMP("append");
        }
        elle::log::logger(std::move(prev));
      }
      else
        elle::unreachable();

      // b. check the files.
      {
        BOOST_TEST_MESSAGE("d contains " << elle::entries(d));
        // This should create five files of less than 600B, but we
        // kept the last three.
        BOOST_CHECK_EQUAL(elle::rotate_versions(base),
                          (std::vector<int>{2, 3, 4}));
        for (int i = 2; i <= 4; ++i)
        {
          auto f = fs::path(elle::print("{}.{}", base.string(), i));
          BOOST_TEST(file_size(f) < 600);
        }
        if (phase == 2)
        {
          auto&& is = std::ifstream(elle::print("{}.4", base.string()));
          BOOST_TEST(boost::contains(elle::content(is), "append"));
        }
      }
    }
  }

  /// Check that FileLogger::base does rename the log file.
  void
  file_rename()
  {
    auto d = elle::filesystem::TemporaryDirectory{};
    auto const log1 = d.path() / "foo.log";
    auto const log2 = d.path() / "bar.log";
    // The expected paths.
    auto const path1 = fs::path{log1.string() + ".0"};
    auto const path2 = fs::path{log2.string() + ".0"};
    {
      auto logger
        = std::make_unique<elle::log::FileLogger>(log1, "DUMP", 512, 3);
      auto& logger_ref = *logger;
      BOOST_CHECK_EQUAL(logger->fstream().path(),
                        log1.string() + ".0");
      auto prev = elle::log::logger(std::move(logger));
      {
        ELLE_DUMP("ping");
        BOOST_CHECK(exists(path1));
        BOOST_CHECK(!exists(path2));
        logger_ref.base(log2);
        ELLE_DUMP("pong");
      }
      elle::log::logger(std::move(prev));
    }
    BOOST_TEST_MESSAGE("d contains " << elle::entries(d.path()));
    BOOST_CHECK(!exists(path1));
    BOOST_CHECK(exists(path2));
    auto const log = elle::content(path2);
    BOOST_TEST(boost::contains(log, "ping"));
    BOOST_TEST(boost::contains(log, "pong"));
  }

  /// Check make_logger.
  void make_logger()
  {
    // file://
    {
      struct Case
      {
        std::string spec;
        std::string base;
        size_t size;
        size_t rotate;
        bool append;
      };
      for (auto const& c:
           {
             Case{"file://foo", "foo", 0, 0, false},
             Case{"file://foo?append", "foo", 0, 0, true},
             Case{"file://foo?size=100KiB", "foo", 100_KiB, 0, false},
             Case{"file://foo?size=1GiB,rotate=10", "foo", 1_GiB, 10, false}
           })
      {
        BOOST_TEST_MESSAGE("Checking: " << c.spec);
        auto const l = elle::log::make_logger(c.spec);
        auto const log = dynamic_cast<elle::log::FileLogger const*>(l.get());
        BOOST_TEST(log);
        BOOST_TEST(log->base() == c.base);
        BOOST_TEST(log->size() == c.size);
        BOOST_TEST(log->rotate() == c.rotate);
        BOOST_TEST(log->append() == c.append);
      }
      BOOST_CHECK_THROW(elle::log::make_logger("file://foo?size=toto"),
                        std::invalid_argument);
      BOOST_CHECK_THROW(elle::log::make_logger("file://foo?append=10"),
                        std::invalid_argument);
      BOOST_CHECK_THROW(elle::log::make_logger("file://foo?zise=10"),
                        std::invalid_argument);
    }
    // stderr://
    {
      auto l = elle::log::make_logger("stderr://");
      BOOST_TEST(dynamic_cast<elle::log::TextLogger*>(l.get()));
      BOOST_CHECK_THROW(elle::log::make_logger("stderr://foo"),
                        std::invalid_argument);
      BOOST_CHECK_THROW(elle::log::make_logger("stderr://?foo"),
                        std::invalid_argument);
    }
    // syslog://
    {
      auto l = elle::log::make_logger("syslog://foo");
      BOOST_TEST(dynamic_cast<elle::log::SysLogger*>(l.get()));
      BOOST_CHECK_THROW(elle::log::make_logger("syslog://foo?size=10"),
                        std::invalid_argument);
    }
    // Check time support.  That's a feature of TextLogger and FileLogger.
    {
      struct Case
      {
        std::string spec;
        bool time;
        bool microsec;
        bool universal;
      };
      for (auto const& base: {"stderr://"s, "file://foo"s})
        for (auto const& c:
             {
               Case{"", false, false, false},
               Case{"?time", true, false, false},
               Case{"?time,microsec", true, true, false},
               Case{"?time,universal", true, false, true},
               Case{"?time,microsec,universal", true, true, true},
              })
        {
          auto spec = base + c.spec;
          BOOST_TEST_MESSAGE("Checking: " << spec);
          auto l = elle::log::make_logger(spec);
          BOOST_TEST_MESSAGE(elle::print("Got: {}", l));
          auto log = get_text_logger(l);
          BOOST_TEST(log);
          BOOST_TEST(log->enable_time() == c.time);
          BOOST_TEST(log->time_microsec() == c.microsec);
          BOOST_TEST(log->time_universal() == c.universal);
        }
    }
  }

  /// Check support of $ELLE_LOG_TARGETS.
  void
  targets()
  {
    auto const d = elle::filesystem::TemporaryDirectory{};
    auto subst = [&](auto&&... as)
      {
        return elle::print(std::forward<decltype(as)>(as)...,
        {
          {"tmp", d.path().string()},
        });
      };

    // d's name on Windows will contain some `\`.  We need to escape
    // them, as ELLE_LOG_TARGETS goes into elle::print.
    auto escape = [](std::string const& s)
      {
        return boost::algorithm::replace_all_copy(s, "\\", "\\\\");
      };

    elle::os::setenv("ELLE_LOG_TARGETS",
                     escape(subst("file://{tmp}/log1?LOG; "
                                  "file://{tmp}/log2?LOG; "
                                  "file://{tmp}/logs?size=100KiB,LOG")));
    // Save previous logger and force the creation of new loggers.
    auto prev = elle::log::logger(nullptr);
    elle::log::logger();
    auto&& l3 = std::ofstream{subst("{tmp}/log3")};
    BOOST_TEST(bool(l3), "l3 is not ok: " << strerror(errno));
    elle::log::logger_add(std::make_unique<elle::log::TextLogger>(l3, "LOG"));
    {
      ELLE_LOG_COMPONENT("component");
      ELLE_LOG("ping");
    }
    l3.close();
    // Restore the previous logger.
    elle::log::logger(std::move(prev));
    elle::os::unsetenv("ELLE_LOG_TARGETS");
    // Check the results.
    BOOST_TEST_MESSAGE("d contains " << elle::entries(d));
    for (auto fn: {"log1", "log2", "log3", "logs.0"})
    {
      auto const f = d.path() / fn;
      BOOST_TEST(exists(f), "missing file: " << f);
    }
  }

  /// Check time display.
  void
  time_()
  {
    // Generate and return a single line of log.
    auto make_log = [](bool microsec, bool universal)
      {
        auto&& os = std::ostringstream{};
        auto&& log = elle::log::TextLogger(os, "DUMP");
        log.enable_time(true);
        log.time_microsec(microsec);
        log.time_universal(universal);
        log.message(elle::log::Logger::Level::dump, // no color, simpler regex
                    elle::log::Logger::Type::info,
                    "component",
                    "message",
                    "file",
                    42,
                    "function");
        auto res = os.str();
        // Remove eol.
        res.resize(res.size() - 1);
        return res;
      };
    // FIXME: check universal vs. local.
    for (auto microsec: {false, true})
      for (auto universal: {false, true})
      {
        auto const msg = make_log(microsec, universal);
        auto const logm = LogMessage(msg);
        BOOST_TEST(logm.microsec.empty() == !microsec);
        BOOST_TEST(logm.component == "component");
        BOOST_TEST(logm.message == "message");
      }
  }
}

/*--------------------------.
| test suite: concurrency.  |
`--------------------------*/

static
void
parallel_write()
{
  std::stringstream output;
  elle::log::TextLogger logger(output);
  logger.component_level("in");
  logger.component_level("out");

  auto action = [](int& counter)
    {
      using namespace boost::posix_time;
      ptime deadline = microsec_clock::local_time() + seconds(10);
      while (microsec_clock::local_time() < deadline && counter < 64)
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

    t1.join();
    t2.join();
  }

  BOOST_CHECK_GE(c1, 64);
  BOOST_CHECK_GE(c2, 64);
}

namespace
{
  /// Check the output of multiline messages.
  void
  multiline()
  {
    auto make_log = []
      {
        std::stringstream output;
        elle::os::setenv("ELLE_LOG_LEVEL", "DUMP");
        auto prev
          = elle::log::logger(std::make_unique<elle::log::TextLogger>(output));
        ELLE_LOG_COMPONENT("multiline");
        ELLE_TRACE("This message\nis\nsplitted\n\ninto\r\n5 lines\n\n\r\n\r\r");
        elle::log::logger(std::move(prev));
        return output.str();
      };
    auto const expected =
      "[multiline] This message\n"
      "            is\n"
      "            splitted\n"
      "            into\n"
      "            5 lines\n";
    BOOST_TEST(make_log() == expected);
    // Check that with time enabled, all the lines have the timestamp.
    {
      auto lines = std::vector<std::string>{};
      elle::os::setenv("ELLE_LOG_TIME", "1");
      auto msg = make_log();
      BOOST_TEST_MESSAGE("msg: " << msg);
      boost::split(lines, msg, boost::is_any_of("\n"));
      // We get an empty line at the end.
      if (lines.back().empty())
        lines.pop_back();
      for (auto const& line: lines)
      {
        auto logm = LogMessage{line};
        BOOST_TEST(!logm.time.empty());
      }
    }
    clear_env();
  }
}

/// Check that we components are displayed with just the needed width.
static
void
component_width()
{
  auto make_log = []
  {
    std::stringstream output;
    elle::log::logger(std::make_unique<elle::log::TextLogger>(output));
    ELLE_LOG_COMPONENT("foo");
    ELLE_TRACE("foo.1")
    {
      ELLE_LOG_COMPONENT("bar");
      ELLE_TRACE("bar.1")
      {
        ELLE_LOG_COMPONENT("quuuuux");
        ELLE_DUMP("quuuuux.1");
      }
      ELLE_TRACE("bar.2");
    }
    ELLE_TRACE("foo.2");
    return output.str();
  };

  elle::os::setenv("ELLE_LOG_LEVEL", "DUMP");
  BOOST_CHECK_EQUAL(make_log(),
                    "[foo] foo.1\n"
                    "[bar]   bar.1\n"
                    "[quuuuux]     quuuuux.1\n"
                    "[  bar  ]   bar.2\n"
                    "[  foo  ] foo.2\n");

  elle::os::setenv("ELLE_LOG_LEVEL", "TRACE");
  BOOST_CHECK_EQUAL(make_log(),
                    "[foo] foo.1\n"
                    "[bar]   bar.1\n"
                    "[bar]   bar.2\n"
                    "[foo] foo.2\n");
}

/// Check the use of context filters: print logs only when nested in
/// some component.
static
void
nested()
{
  auto make_log = []
  {
    std::stringstream output;
    elle::log::logger(std::make_unique<elle::log::TextLogger>(output));
    ELLE_LOG_COMPONENT("foo");
    ELLE_TRACE("foo.1")
    {
      ELLE_LOG_COMPONENT("bar");
      ELLE_TRACE("bar.1")
      {
        ELLE_LOG_COMPONENT("baz");
        ELLE_TRACE("baz.1");
        ELLE_TRACE("baz.2");
      }
      ELLE_TRACE("bar.2");
    }
    ELLE_TRACE("foo.2")
    {
      ELLE_LOG_COMPONENT("baz");
      ELLE_TRACE("baz.3");
      ELLE_TRACE("baz.4");
    }
    ELLE_TRACE("foo.3");
    return output.str();
  };

  elle::os::setenv("ELLE_LOG_LEVEL", "TRACE");
  BOOST_CHECK_EQUAL(make_log(),
                    "[foo] foo.1\n"
                    "[bar]   bar.1\n"
                    "[baz]     baz.1\n"
                    "[baz]     baz.2\n"
                    "[bar]   bar.2\n"
                    "[foo] foo.2\n"
                    "[baz]   baz.3\n"
                    "[baz]   baz.4\n"
                    "[foo] foo.3\n");

  elle::os::setenv("ELLE_LOG_LEVEL", "baz:TRACE");
  BOOST_CHECK_EQUAL(make_log(),
                    "[baz]     baz.1\n"
                    "[baz]     baz.2\n"
                    "[baz]   baz.3\n"
                    "[baz]   baz.4\n");

  elle::os::setenv("ELLE_LOG_LEVEL", "bar baz:TRACE");
  BOOST_CHECK_EQUAL(make_log(),
                    "[baz]     baz.1\n"
                    "[baz]     baz.2\n");
}

static
void
trim()
{
  std::stringstream output;
  elle::os::setenv("ELLE_LOG_LEVEL", "DUMP");
  elle::log::logger(std::make_unique<elle::log::TextLogger>(output));
  ELLE_LOG_COMPONENT("trim");
  ELLE_TRACE("   \n\t\t\tThis message is trimmed !    \n\n\r\n\r\r\t ");
  BOOST_TEST(output.str() == "[trim] This message is trimmed !\n");
}

/*---------------------.
| test suite: format.  |
`---------------------*/

namespace
{
  void
  error()
  {
    ELLE_LOG_COMPONENT("error");
    {
      std::stringstream output;
      auto prev = elle::log::logger(std::make_unique<elle::log::TextLogger>(output));
      {
        // We are passing through already executed code with a different logger
        ELLE_LOG_COMPONENT("elle.printf");
        ELLE_LOG("force component creation");
      }
      ELLE_LOG("invalid log", 42);
      ELLE_LOG("invalid log %s");
      BOOST_CHECK_GT(output.str().size(), 0);
      elle::log::logger(std::move(prev));
    }
    {
      std::stringstream output;
      elle::log::detail::debug_formats(true);
      auto prev = elle::log::logger(std::make_unique<elle::log::TextLogger>(output));
      {
        // We are passing through already executed code with a different logger
        ELLE_LOG_COMPONENT("elle.printf");
        ELLE_LOG("force component creation");
      }
      BOOST_CHECK_THROW(ELLE_LOG("invalid log", 42), elle::Exception);
      BOOST_CHECK_THROW(ELLE_LOG("invalid log %s"), elle::Exception);
      BOOST_CHECK(!output.str().empty());
      elle::log::detail::debug_formats(false);
      elle::log::logger(std::move(prev));
    }
  }
}

ELLE_TEST_SUITE()
{
  elle::log::detail::debug_formats(false);
  elle::os::setenv("ELLE_LOG_COLOR", "1", false);

  auto& suite = boost::unit_test::framework::master_test_suite();
  auto logger = BOOST_TEST_SUITE("logger");
  {
    suite.add(logger);
    logger->add(BOOST_TEST_CASE(message_test));
    logger->add(BOOST_TEST_CASE(environment_format_test));
    logger->add(BOOST_TEST_CASE(composite));
    logger->add(BOOST_TEST_CASE(file));
    logger->add(BOOST_TEST_CASE(file_rename));
    logger->add(BOOST_TEST_CASE(make_logger));
    logger->add(BOOST_TEST_CASE(targets));
    logger->add(BOOST_TEST_CASE(time_));
  }

#ifndef ELLE_ANDROID
  auto concurrency = BOOST_TEST_SUITE("concurrency");
  {
    suite.add(concurrency);
    concurrency->add(BOOST_TEST_CASE(parallel_write));
  }

  auto format = BOOST_TEST_SUITE("format");
  {
    suite.add(format);
    format->add(BOOST_TEST_CASE(error));
    format->add(BOOST_TEST_CASE(multiline));
    format->add(BOOST_TEST_CASE(trim));
    format->add(BOOST_TEST_CASE(component_width));
    format->add(BOOST_TEST_CASE(nested));
  }
#endif
}
