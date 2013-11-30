#include <sstream>
#include <string>

#include <elle/container/initializer_list.hh>
#include <elle/container/map.hh>
#include <elle/container/set.hh>
#include <elle/container/list.hh>
#include <elle/container/vector.hh>
#include <elle/log.hh>
#include <elle/print.hh>
#include <elle/test.hh>

ELLE_LOG_COMPONENT("containers");

static
void
printable_init()
{
  std::initializer_list<int> i{1, 2, 3, 4, 5};
  ELLE_LOG("initializer_list: %s", i);
  BOOST_CHECK_EQUAL(elle::sprint(i), "{1, 2, 3, 4, 5}");
}

static
void
printable_pair()
{
  std::pair<std::string, int> p{"toto", 1};
  ELLE_LOG("pair: %s", p);
  BOOST_CHECK_EQUAL(elle::sprint(p), "toto: 1");
}

static
void
printable_map()
{
  std::map<std::string, int> m{
    {"toto", 1},
    {"tata", 2},
  };
  ELLE_LOG("map: %s", m);
  BOOST_CHECK_EQUAL(elle::sprint(m), "{tata: 2, toto: 1}");
}

static
void
printable_unordered_map()
{
  std::unordered_map<std::string, int> m{
    {"toto", 1},
    {"tata", 2},
  };
  ELLE_LOG("unordered_map: %s", m);
  // unordered_map representation is unpredictable. While it's not test with
  // regex, the best way is to compare representation size to the expected one.
  size_t size = std::string("{tata: 2, toto: 1}").length();
  BOOST_CHECK_EQUAL(elle::sprint(m).length(), size);
}

static
void
printable_set()
{
  std::set<int> s{0, 1, 2};
  ELLE_LOG("set: %s", s);
  BOOST_CHECK_EQUAL(elle::sprint(s), "{0, 1, 2}");
}

static
void
printable_unordered_set()
{
  std::unordered_set<int> s{2, 1, 0};
  ELLE_LOG("unordered_set: %s", s);
  // unordered_set representation is unpredictable. While it's not test with
  // regex, the best way is to compare representation size to the expected one.
  size_t size = std::string("{0, 1, 2}").length();
  BOOST_CHECK_EQUAL(elle::sprint(s).length(), size);
}

static
void
printable_list()
{
  std::list<int> l{0, 1, 2};
  ELLE_LOG("list: %s", l);
  BOOST_CHECK_EQUAL(elle::sprint(l), "(0, 1, 2)");
}

static
void
printable_vector()
{
  std::vector<int> v{0, 1, 2};
  ELLE_LOG("vector: %s", v);
  BOOST_CHECK_EQUAL(elle::sprint(v), "[0, 1, 2]");
}

static
void
printable_vectorpair()
{
  std::vector<std::pair<std::string, int>> v{
    {"toto", 1},
    {"tata", 2},
  };
  ELLE_LOG("vector: %s", v);
  BOOST_CHECK_EQUAL(elle::sprint(v), "[toto: 1, tata: 2]");
}

static
void
printable_vectorvectorpair()
{
  std::vector<std::vector<std::pair<std::string, int>>> v{
    { {"toto", 1}, {"tata", 2},},
    { {"titi", 3}, {"tutu", 4},},
  };
  ELLE_LOG("vector: %s", v);

  BOOST_CHECK_EQUAL(elle::sprint(v), "[[toto: 1, tata: 2], [titi: 3, tutu: 4]]");
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(printable_unordered_set));
  suite.add(BOOST_TEST_CASE(printable_vector));
  suite.add(BOOST_TEST_CASE(printable_pair));
  suite.add(BOOST_TEST_CASE(printable_map));
  suite.add(BOOST_TEST_CASE(printable_unordered_map));
  suite.add(BOOST_TEST_CASE(printable_init));
  suite.add(BOOST_TEST_CASE(printable_vectorpair));
  suite.add(BOOST_TEST_CASE(printable_set));
  suite.add(BOOST_TEST_CASE(printable_vectorvectorpair));
  suite.add(BOOST_TEST_CASE(printable_list));
}

