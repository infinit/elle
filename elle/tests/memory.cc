#define BOOST_TEST_MODULE memory
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <elle/memory.hh>
#include <tuple>

BOOST_AUTO_TEST_CASE(make_unique1)
{
  std::unique_ptr<std::tuple<int, char, double>> ptr =
    elle::make_unique<std::tuple<int, char, double>>(1, 'c', 1.0f);
  BOOST_CHECK(std::get<0>(*ptr) == 1);
  BOOST_CHECK(std::get<1>(*ptr) == 'c');
  BOOST_CHECK(std::get<2>(*ptr) == 1.0f);
}

BOOST_AUTO_TEST_CASE(make_unique2)
{
  std::unique_ptr<std::tuple<>> ptr =
    elle::make_unique<std::tuple<>>();
}
