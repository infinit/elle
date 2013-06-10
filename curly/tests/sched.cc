#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_IGNORE_SIGCHLD
#define BOOST_TEST_IGNORE_NON_ZERO_CHILD_CODE
#define BOOST_TEST_MODULE curly_sched
#include <boost/test/unit_test.hpp>

#include <curly/curly_sched.cc>
#include <elle/printf.hh>
#include <elle/system/Process.hh>

#include <reactor/scheduler.hh>
#include <reactor/thread.hh>
#include <reactor/Scope.hh>

#include <elle/container/vector.hh>

#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>

BOOST_AUTO_TEST_CASE(simple_test)
{
  reactor::Scheduler sched;

  std::string http_path =
    boost::unit_test::framework::master_test_suite().argv[1];

  auto pc = elle::system::process_config(elle::system::normal_config);
  elle::system::Process p{std::move(pc),
    "python3",
    {"-mhttp.server", "56789"}
  };
  sleep(1);

  auto run_test = [&] {
    std::vector<std::string> v;

    reactor::Thread* current = sched.current();
    auto fn = [&]
    {
      std::stringstream ss;
      auto get = curly::make_get();

      get.output(ss);
      get.option(CURLOPT_VERBOSE, 0);
      get.url("http://127.0.0.1:56789/drake");
      //get.url("http://fabien.le.boute-en-tra.in");
      curly::sched_request req(sched, get);
      req.run();
      std::cout << ss.str() << std::endl;
      v.push_back(ss.str());
    };

    reactor::Thread t0(sched, "test-0", fn);
    reactor::Thread t1(sched, "test-1", fn);
    reactor::Thread t2(sched, "test-2", fn);
    reactor::Thread t3(sched, "test-3", fn);

    current->wait(t0);
    current->wait(t1);
    current->wait(t2);
    current->wait(t3);

    std::set<std::string> s;
    std::move(begin(v), end(v), std::inserter(s, s.begin()));
    BOOST_CHECK_EQUAL(s.size(), 1);
  };
  reactor::Thread main(sched, "main", run_test);
  sched.run();
  p.interrupt();
}
