#include <sstream>

#include <elle/json/json.hh>
#include <elle/test.hh>
#include <elle/log.hh>

ELLE_LOG_COMPONENT("Test");

static
void
read_int()
{
  std::stringstream input("42");
  auto json = elle::json::read(input);
  BOOST_CHECK_EQUAL(boost::any_cast<int64_t>(json), 42);
}

static
void
read_long()
{
  std::stringstream input("9437196296");
  auto json = elle::json::read(input);
  BOOST_CHECK_EQUAL(boost::any_cast<int64_t>(json), 9437196296);
}

static
bool
cycle(boost::any& v)
{
  try
  {
    std::stringstream s;
    elle::json::write(s, v);
    elle::json::read(s);
    return true;
  }
  catch (std::exception const& e)
  {
    ELLE_WARN("%s", e.what());
    return false;
  }
}

static
void
write_ints()
{
  // test our test
  {
    struct Foo{int bar;};
    boost::any v = Foo{};
    BOOST_CHECK(!cycle(v));
  }
  boost::any v = 1;
  BOOST_CHECK(cycle(v));
  v = 1L;
  BOOST_CHECK(cycle(v));
  v = 1LL;
  BOOST_CHECK(cycle(v));
  v = 1UL;
  BOOST_CHECK(cycle(v));
  v = 1ULL;
  BOOST_CHECK(cycle(v));
  v = (unsigned long)1;
  BOOST_CHECK(cycle(v));
  v = (unsigned long long)1;
  BOOST_CHECK(cycle(v));
  v = (long)1;
  BOOST_CHECK(cycle(v));
  v = (long long)1;
  BOOST_CHECK(cycle(v));
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
  BOOST_CHECK_EQUAL(boost::any_cast<int64_t>(object["pastis"]), 51);
  BOOST_CHECK_EQUAL(boost::any_cast<std::string>(object["Paul"]), "Ricard");
}

static
void
null()
{
  std::stringstream input("null");
  auto json = elle::json::read(input);
  boost::any_cast<elle::json::NullType>(json);
  BOOST_CHECK_EQUAL(elle::json::pretty_print(json), "null");
}

static
void
read_utf_8()
{
  std::stringstream input("{\"utf-8\": \"Средня Азиdoc\"}");
  auto object = boost::any_cast<elle::json::Object>(elle::json::read(input));
  BOOST_CHECK_EQUAL(object.size(), 1);
  BOOST_CHECK(object.find("utf-8") != object.end());
  BOOST_CHECK_EQUAL(boost::any_cast<std::string>(object["utf-8"]),
                    "Средня Азиdoc");
}

static
void
read_escaped_utf_8()
{
  std::stringstream input(
    "{\"utf-8\": \"\\u0421\\u0440\\u0435\\u0434\\u043d\\u044f \\u0410\\u0437\\u0438doc\"}");
  auto object = boost::any_cast<elle::json::Object>(elle::json::read(input));
  BOOST_CHECK_EQUAL(object.size(), 1);
  BOOST_CHECK(object.find("utf-8") != object.end());
  BOOST_CHECK_EQUAL(boost::any_cast<std::string>(object["utf-8"]),
                    "Средня Азиdoc");
}

static
void
write_utf_8()
{
  std::stringstream output;
  elle::json::Object input;
  input["utf-8"] = std::string("Средня Азиdoc");
  elle::json::write(output, input);
  BOOST_CHECK_EQUAL(output.str(), "{\"utf-8\":\"Средня Азиdoc\"}\n");
}

static
void
pretty_printer_utf_8()
{
  elle::json::Object object;
  std::string name = "Bôb";
  object["utf-8"] = name;
  auto pretty_str = elle::json::pretty_print(object);
  std::stringstream stream(pretty_str);
  auto read_object =
    boost::any_cast<elle::json::Object>(elle::json::read(stream));
  BOOST_CHECK_EQUAL(boost::any_cast<std::string>(read_object["utf-8"]), name);
}

ELLE_TEST_SUITE()
{
  auto timeout = 3;
  auto& suite = boost::unit_test::framework::master_test_suite();

  suite.add(BOOST_TEST_CASE(read_int), 0, timeout);
  suite.add(BOOST_TEST_CASE(read_long), 0, timeout);
  suite.add(BOOST_TEST_CASE(write_ints), 0, timeout);
  suite.add(BOOST_TEST_CASE(read_object), 0, timeout);
  suite.add(BOOST_TEST_CASE(null), 0, timeout);
  suite.add(BOOST_TEST_CASE(read_utf_8), 0, timeout);
  suite.add(BOOST_TEST_CASE(read_escaped_utf_8), 0, timeout);
  suite.add(BOOST_TEST_CASE(write_utf_8), 0, timeout);
  suite.add(BOOST_TEST_CASE(pretty_printer_utf_8), 0, timeout);
}
