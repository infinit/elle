#define BOOST_TEST_MODULE Lazy
#include <boost/test/unit_test.hpp>

#include <sstream>

#include <elle/Lazy.hh>

BOOST_AUTO_TEST_CASE(evaluation)
{
  int evaluated = 0;

  elle::Lazy<std::vector<int>> lazy([&evaluated]
                                    {
                                      ++evaluated;
                                      return std::vector<int>{1, 2,};
                                    });

  BOOST_CHECK_EQUAL(evaluated, 0);
  lazy->push_back(3);
  BOOST_CHECK_EQUAL(evaluated, 1);
  lazy->push_back(4);
  BOOST_CHECK_EQUAL(evaluated, 1);
  BOOST_CHECK_EQUAL(lazy->size(), 4);
}

BOOST_AUTO_TEST_CASE(print)
{
  elle::Lazy<int> lazy([]
                       {
                         return 21 * 2;
                       });
  std::stringstream stream;
  stream << lazy;
  BOOST_CHECK_EQUAL(stream.str(), "42");
}
