#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <boost/algorithm/string.hpp>

#include <curly/curly.hh>
#include <elle/system/Process.hh>
#include <elle/assert.hh>
#include <elle/print.hh>

#include <atomic>
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <memory>

class httpd
{
private:
  std::unique_ptr<elle::system::Process> _httpd;
public:

  std::string port;
  httpd()
  {
    this->port = "65458";
    auto pc = elle::system::pipe_output_config;
    this->_httpd.reset(new elle::system::Process{
                       pc, "python3", {"-mhttp.server", this->port}
                       });
    sleep(1);
  }

  ~httpd()
  {
    this->_httpd->interrupt();
  }

  std::string
  root_url()
  {
    // For some reason, resolution sometimes times out under valgrind, hence the
    // IP instead of localhost. This is unrelated to curly since the simplest
    // Curl request yields the same behavior.
    return elle::sprintf("http://127.0.0.1:%s/", this->port);
  }
};

httpd global_server;

static
void
simple()
{
  std::string page = curly::get(global_server.root_url());
  BOOST_CHECK(page.empty() == false);
}

static
void
complex()
{
  auto rc = curly::make_get();

  rc.url(global_server.root_url());
  curly::request r(std::move(rc));
  BOOST_CHECK(r.code() == 200);
}

static
void
timed()
{
  auto rc = curly::make_get();

  rc.url(global_server.root_url());
  curly::request r(std::move(rc));
  BOOST_CHECK(r.code() == 200);
  std::cout << "time is " << r.time().count() << std::endl;
  BOOST_CHECK(r.time() != std::chrono::duration<double>(0));
}

static
void
threaded()
{
  int const number_of_thread = 3;
  std::vector<std::thread> vt;

  std::mutex mutex;
  auto fn = [&] {
    for (int i = 0; i < 10 * number_of_thread; ++i)
    {
      auto rc = curly::make_get();
      rc.url(global_server.root_url());
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
}

static
bool
test_suite()
{
  auto& ts = boost::unit_test::framework::master_test_suite();
  ts.add(BOOST_TEST_CASE(simple), 0, 20);
  ts.add(BOOST_TEST_CASE(complex), 0, 20);
  ts.add(BOOST_TEST_CASE(timed), 0, 20);
  ts.add(BOOST_TEST_CASE(threaded), 0, 20);
  return true;
}

int
main(int argc, char** argv)
{
  return ::boost::unit_test::unit_test_main(test_suite, argc, argv);
}
