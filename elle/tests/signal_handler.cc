#include <iostream>
#include <csignal>

#include <CrashReporter.hh>
#include <reactor/scheduler.hh>
#include <reactor/sleep.hh>


#include <boost/property_tree/info_parser.hpp>

#define COUNT 10

static reactor::Scheduler sched{};

struct Increaser
{
  static int count;

  void
  operator ()(int sig)
  {
    std::cerr << sig << std::endl;
    ++Increaser::count;
  }
};

int Increaser::count = 0;

int
 do_something()
{
  reactor::Sleep s(sched, boost::posix_time::seconds(5));
  s.run();

  return 1;
}

int
kill_it_with_fire()
{
  {
    raise(SIGSEGV);
    reactor::Sleep s(sched, boost::posix_time::seconds(1));
    s.run();

  }
  return 0;
}

int
main(void)
{
  reactor::VThread<int> main(sched,
                             "Infinit main",
                             [] () -> int {
                               std::vector<std::unique_ptr<elle::signal::ScopedGuard>> guards;
                               for (unsigned int i = 0; i < COUNT; ++i)
                                 guards.push_back(std::unique_ptr<elle::signal::ScopedGuard>{
                                     new elle::signal::ScopedGuard{sched, {SIGSEGV}, Increaser()}});
                               for (unsigned int i = 0; i < COUNT; ++i)
                                 guards.push_back(std::unique_ptr<elle::signal::ScopedGuard>{
                                     new elle::signal::ScopedGuard{sched, {SIGINT}, Increaser()}});

                               return do_something();
                             });

  reactor::VThread<int> kill(sched,
                             "raiser",
                             boost::bind(&kill_it_with_fire));

  sched.run();

  if (Increaser::count == COUNT)
    std::cout << "tests done." << std::endl;

  return 0;
}
