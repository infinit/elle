#include <sstream>

#include <elle/json/json.hh>
#include <elle/test.hh>

static
void
read_int()
{
  std::stringstream input("42");
  auto json = elle::json::read(input);
  BOOST_CHECK_EQUAL(boost::any_cast<int>(json), 42);
}

static
void
read_object()
{
  std::stringstream input("{\"pastis\": 51, \"Paul\": \"Ricard\"}");
  auto object = boost::any_cast<elle::json::Object>(elle::json::read(input));
  BOOST_CHECK_EQUAL(object.size(), 2);
  BOOST_CHECK(object.find("pastis") != object.end());
  BOOST_CHECK(object.find("Paul") != object.end());
  BOOST_CHECK_EQUAL(boost::any_cast<int>(object["pastis"]), 51);
  BOOST_CHECK_EQUAL(boost::any_cast<std::string>(object["Paul"]), "Ricard");
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();

  suite.add(BOOST_TEST_CASE(read_int), 0, 3);
  suite.add(BOOST_TEST_CASE(read_object), 0, 3);
}
