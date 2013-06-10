#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE curly
#include <boost/test/unit_test.hpp>

#include <curly/curly.hh>

#include <iostream>

BOOST_AUTO_TEST_CASE(simple)
{
  std::string page = curly::get("http://meta.api.development.infinit.io:12345/");
  BOOST_CHECK(page.empty() == false);
}

BOOST_AUTO_TEST_CASE(complex)
{
  auto rc = curly::make_get();

  rc.url("http://meta.api.development.infinit.io:12345/");
  curly::request r(rc);
  BOOST_CHECK(r.code() == 200);
}

BOOST_AUTO_TEST_CASE(timed)
{
  auto rc = curly::make_get();

  rc.url("http://meta.api.development.infinit.io:12345/");
  curly::request r(rc);
  BOOST_CHECK(r.code() == 200);
  std::cout << "time is " << r.time().count() << std::endl;
  BOOST_CHECK(r.time() != std::chrono::duration<double>(0));
}
