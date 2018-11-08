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
  BOOST_CHECK(json == 42);
}

static
void
read_long()
{
  std::stringstream input("9437196296");
  auto json = elle::json::read(input);
  BOOST_CHECK(json == 9437196296);
}

template <typename T>
static
bool
cycle(T&& v)
{
  try
  {
    std::stringstream s;
    elle::json::write(s, std::forward<T>(v));
    elle::json::read(s);
    return true;
  }
  catch (elle::Error const& e)
  {
    ELLE_WARN("%s", e.what());
    return false;
  }
}

static
void
write_ints()
{
  BOOST_CHECK(cycle(1));
  BOOST_CHECK(cycle(1L));
  BOOST_CHECK(cycle(1LL));
  BOOST_CHECK(cycle(1UL));
  BOOST_CHECK(cycle(1ULL));
  BOOST_CHECK(cycle((unsigned long)1));
  BOOST_CHECK(cycle((unsigned long long)1));
  BOOST_CHECK(cycle((long)1));
  BOOST_CHECK(cycle((long long)1));
}

static
void
read_object()
{
  std::stringstream input("{\"pastis\": 51, \"Paul\": \"Ricard\"}");
  auto object = elle::json::read(input);
  BOOST_CHECK_EQUAL(object.size(), 2);
  BOOST_CHECK(object.find("pastis") != object.end());
  BOOST_CHECK(object.find("Paul") != object.end());
  BOOST_CHECK(object["pastis"] == 51);
  BOOST_CHECK(object["Paul"] == "Ricard");
}

static
void
null()
{
  std::stringstream input("null");
  auto json = elle::json::read(input);
  BOOST_CHECK(json.is_null());
  BOOST_CHECK(elle::json::pretty_print(json) == "null");
}

static
void
read_utf_8()
{
  std::stringstream input("{\"utf-8\": \"Средня Азиdoc\"}");
  auto object = elle::json::read(input);
  BOOST_CHECK_EQUAL(object.size(), 1);
  BOOST_CHECK(object.find("utf-8") != object.end());
  BOOST_CHECK(object["utf-8"] == "Средня Азиdoc");
}

static
void
read_escaped_utf_8()
{
  std::stringstream input(
    "{\"utf-8\": \"\\u0421\\u0440\\u0435\\u0434\\u043d\\u044f \\u0410\\u0437\\u0438doc\"}");
  auto object = elle::json::read(input);
  BOOST_CHECK_EQUAL(object.size(), 1);
  BOOST_CHECK(object.find("utf-8") != object.end());
  BOOST_CHECK(object["utf-8"] == "Средня Азиdoc");
}

static
void
write_utf_8()
{
  std::stringstream output;
  elle::json::Json input;
  input["utf-8"] = std::string("Средня Азиdoc");
  elle::json::write(output, input);
  BOOST_CHECK_EQUAL(output.str(), "{\"utf-8\":\"Средня Азиdoc\"}\n");
}

static
void
pretty_printer_utf_8()
{
  elle::json::Json object;
  std::string name = "Bôb";
  object["utf-8"] = name;
  auto pretty_str = elle::json::pretty_print(object);
  std::stringstream stream(pretty_str);
  auto read_object = elle::json::read(stream);
  BOOST_CHECK(read_object["utf-8"] == name);
}

static
void
pretty_print_stream_states()
{
  std::stringstream s;
  BOOST_CHECK(s.width() == 0);
  elle::json::write(s, R"({"i" : 0})"_json, false, true);
  BOOST_CHECK(s.width() == 0);
}

static
void
multiple_values()
{
  std::stringstream input("{\"i\": 0}{\"i\": 1}");
  BOOST_CHECK(elle::json::read(input)["i"] == 0);
  BOOST_CHECK(elle::json::read(input)["i"] == 1);
}

static
void
errors()
{
  // Parse error
  BOOST_CHECK_THROW(elle::json::read("{0:1}"), elle::Error);
  // Leftover in string
  BOOST_CHECK_THROW(elle::json::read("{\"i\": 0}{"), elle::Error);
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
  suite.add(BOOST_TEST_CASE(pretty_print_stream_states), 0, timeout);
  suite.add(BOOST_TEST_CASE(multiple_values), 0, timeout);
  suite.add(BOOST_TEST_CASE(errors), 0, timeout);
}
