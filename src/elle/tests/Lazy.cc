#include <sstream>

#include <elle/Lazy.hh>
#include <elle/test.hh>

static
void
evaluation()
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

static
void
print()
{
  elle::Lazy<int> lazy([]
                       {
                         return 21 * 2;
                       });
  std::stringstream stream;
  stream << lazy;
  BOOST_CHECK_EQUAL(stream.str(), "42");
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();

  suite.add(BOOST_TEST_CASE(evaluation));
  suite.add(BOOST_TEST_CASE(print));
}
