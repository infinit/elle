#define BOOST_TEST_DYN_LINK

#include "reactor.hh"

#include <boost/test/unit_test.hpp>

#include <elle/log/TextLogger.hh>

#include <reactor/thread.hh>

reactor::Scheduler* sched = 0;
elle::log::TextLogger* logger;
std::stringstream ss, res;

Fixture::Fixture()
{
  sched = new reactor::Scheduler;
  logger = new elle::log::TextLogger(ss);
  logger->component_enabled("Test");
}

Fixture::~Fixture()
{
  delete sched;
  delete logger;
  sched = 0;
}

void
gen_message(const std::string& thread_name,
            const int& indent_level)
{
  std::string msg;
  for (int i=0; i<indent_level; i++)
    logger->indent();

  msg = "Test message from ";
  msg += thread_name;
  logger->message(elle::log::Logger::Level::log,
                  elle::log::Logger::Type::info,
                  "Test",
                  msg);
  if (thread_name == "Thread 1")
    BOOST_CHECK_EQUAL(logger->indentation(), 2);
  if (thread_name == "Thread 2")
    BOOST_CHECK_EQUAL(logger->indentation(), 4);

  sched->current()->yield();

  msg = "Another message from ";
  msg += thread_name;
  logger->message(elle::log::Logger::Level::log,
                  elle::log::Logger::Type::info,
                  "Test",
                  msg);
  if (thread_name == "Thread 1")
    BOOST_CHECK_EQUAL(logger->indentation(), 2);
  if (thread_name == "Thread 2")
    BOOST_CHECK_EQUAL(logger->indentation(), 4);

  sched->current()->yield();

  if (thread_name == "Thread 2")
    logger->unindent();
  msg = "A third message from ";
  msg += thread_name;
  logger->message(elle::log::Logger::Level::debug,
                  elle::log::Logger::Type::warning,
                  "Test",
                  msg);
  if (thread_name == "Thread 1")
    BOOST_CHECK_EQUAL(logger->indentation(), 2);
  if (thread_name == "Thread 2")
    BOOST_CHECK_EQUAL(logger->indentation(), 3);
}

static
void
scheduler_log_test()
{
  Fixture f;

  reactor::Thread t1(*sched, "Thread1", std::bind(gen_message, std::ref("Thread 1"), 2));
  reactor::Thread t2(*sched, "Thread2", std::bind(gen_message, std::ref("Thread 2"), 4));

  sched->run();

  res << "[Test] [Thread1]   Test message from Thread 1\n"
      << "[Test] [Thread2]       Test message from Thread 2\n"
      << "[Test] [Thread1]   Another message from Thread 1\n"
      << "[Test] [Thread2]       Another message from Thread 2\n"
      << "[33;01;33m[Test] [Thread1]   A third message from Thread 1\n[0m"
      << "[33;01;33m[Test] [Thread2]     A third message from Thread 2\n[0m";

  BOOST_CHECK_EQUAL(ss.str(), res.str());
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
      reactor::Scheduler sched;
      reactor::Thread t(sched, "logger", [&logger,&counter]()
        {
          using namespace boost::posix_time;
          ptime deadline = microsec_clock::local_time() + seconds(1);
          while (microsec_clock::local_time() < deadline)
          {
            logger.indent();
            logger.message(elle::log::Logger::Level::log,
                           elle::log::Logger::Type::info,
                           "out",
                           "out");
            logger.indent();
            logger.message(elle::log::Logger::Level::log,
                           elle::log::Logger::Type::error,
                           "in",
                           "in");
            logger.unindent();
            logger.unindent();
            ++counter;
          }
        });
      sched.run();
    };

  std::list<std::thread> threads;
  std::list<int> counters;
  for (int i = 0; i < 64; ++i)
  {
    counters.push_back(0);
    int& counter = counters.back();
    threads.push_back(std::thread([&](){ action(counter); }));
  }

  for (auto& thread: threads)
    thread.join();

  for (auto counter: counters)
    BOOST_CHECK_GT(counter, 64);
}

static
bool
test_suite()
{
  boost::unit_test::test_suite* sched_log = BOOST_TEST_SUITE("Sched Logger");
  boost::unit_test::framework::master_test_suite().add(sched_log);
  sched_log->add(BOOST_TEST_CASE(std::bind(scheduler_log_test)));
  sched_log->add(BOOST_TEST_CASE(std::bind(parallel_write)));

  return true;
}

int
main(int argc, char** argv)
{
  return ::boost::unit_test::unit_test_main(test_suite, argc, argv);
}
