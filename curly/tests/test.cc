#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE curly
#include <boost/test/unit_test.hpp>

#include <curly/curly.hh>
#include <elle/system/Process.hh>
#include <elle/assert.hh>

#include <atomic>
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>

BOOST_AUTO_TEST_CASE(simple)
{
  std::string page = curly::get("http://meta.api.development.infinit.io:12345/");
  BOOST_CHECK(page.empty() == false);
}

BOOST_AUTO_TEST_CASE(complex)
{
  auto rc = curly::make_get();

  rc.url("http://meta.api.development.infinit.io:12345/");
  curly::request r(std::move(rc));
  BOOST_CHECK(r.code() == 200);
}

BOOST_AUTO_TEST_CASE(timed)
{
  auto rc = curly::make_get();

  rc.url("http://meta.api.development.infinit.io:12345/");
  curly::request r(std::move(rc));
  BOOST_CHECK(r.code() == 200);
  std::cout << "time is " << r.time().count() << std::endl;
  BOOST_CHECK(r.time() != std::chrono::duration<double>(0));
}

BOOST_AUTO_TEST_CASE(threaded_lock)
{
  int const number_of_thread = 15;
  std::vector<std::thread> vt;

  auto pc = elle::system::normal_config;
  elle::system::Process server{std::move(pc),
    "python3", {"-mhttp.server", "8081"}};
  sleep(1);
  std::mutex mutex;
  auto fn = [&]
  {
    for (int i = 0; i < 10 * number_of_thread; ++i)
    {
      std::lock_guard<std::mutex> guard{mutex};
      std::string page = curly::get("http://infinit.im:12345/");
      BOOST_CHECK(page.empty() == false);
    }
  };
  for (int i = 0; i < number_of_thread; ++i)
    vt.emplace_back(fn);

  for (auto& th: vt)
    th.join();
  server.interrupt();
}

BOOST_AUTO_TEST_CASE(threaded)
{
  int const number_of_thread = 3;
  std::vector<std::thread> vt;

  elle::system::Process server{"python3", {"-mhttp.server", "8080"}};
  sleep(1);
  std::mutex mutex;

  auto fn = [&] {
    for (int i = 0; i < 10 * number_of_thread; ++i)
    {
      auto rc = curly::make_get();
      rc.url("http://localhost:8080/");
      rc.option(CURLOPT_CONNECTTIMEOUT, 300000);
      rc.option(CURLOPT_TIMEOUT, 300000);
      rc.header("Connection", "close");

      std::string page;
      {
        std::stringstream ss;
        rc.output(ss);
        curly::request r(std::move(rc));
        page = ss.str();
      }

      {
        std::lock_guard<std::mutex> guard{mutex};
        BOOST_CHECK_EQUAL(page.empty(), false);
      }
    }
  };

  for (int i = 0; i < number_of_thread; ++i)
    vt.emplace_back(fn);

  for (auto& th: vt)
    th.join();
  server.interrupt();
}
