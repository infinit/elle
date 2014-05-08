#include <deque>
#include <list>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#include <elle/attribute.hh>
#include <elle/container/deque.hh>
#include <elle/container/list.hh>
#include <elle/container/map.hh>
#include <elle/container/vector.hh>
#include <elle/serialization/json.hh>
#include <elle/serialization/json/MissingKey.hh>
#include <elle/serialization/json/TypeError.hh>
#include <elle/test.hh>

template <typename Format>
static
void
integer()
{
  std::stringstream stream;
  {
    typename Format::SerializerOut output(stream);
    int i = 42;
    output.serialize("int", i);
  }
  {
    typename Format::SerializerIn input(stream);
    int i = 0;
    input.serialize("int", i);
    BOOST_CHECK_EQUAL(i, 42);
  }
}

class Point
{
public:
  Point()
    : _x(0)
    , _y(0)
  {}

  Point(int x, int y)
    : _x(x)
    , _y(y)
  {}

  Point(elle::serialization::Serializer& s)
    : Point()
  {
    this->serialize(s);
  }

  bool
  operator ==(Point const& other) const
  {
    return this->x() == other.x() && this->y() == other.y();
  }

  void
  serialize(elle::serialization::Serializer& s)
  {
    s.serialize("x", this->_x);
    s.serialize("y", this->_y);
  }

  ELLE_ATTRIBUTE_R(int, x);
  ELLE_ATTRIBUTE_R(int, y);
};

static
std::ostream&
operator << (std::ostream& out, Point const& p)
{
  out << "Point(" << p.x() << ", " << p.y() << ")";
  return out;
}

template <typename Format>
static
void
object()
{
  std::stringstream stream;
  {
    typename Format::SerializerOut output(stream);
    Point p(42, 51);
    p.serialize(output);
  }
  {
    typename Format::SerializerIn input(stream);
    Point p(input);
    BOOST_CHECK_EQUAL(p, Point(42, 51));
  }
}

class Line
{
public:
  Line()
    : _start()
    , _end()
  {}

  Line(Point start, Point end)
    : _start(start)
    , _end(end)
  {}

  Line(elle::serialization::Serializer& s)
    : Line()
  {
    this->serialize(s);
  }

  bool
  operator ==(Line const& other) const
  {
    return this->start() == other.start() && this->end() == other.end();
  }

  void
  serialize(elle::serialization::Serializer& s)
  {
    s.serialize("start", this->_start);
    s.serialize("end", this->_end);
  }

  ELLE_ATTRIBUTE_R(Point, start);
  ELLE_ATTRIBUTE_R(Point, end);
};

static
std::ostream&
operator << (std::ostream& out, Line const& l)
{
  out << "Line(" << l.start() << ", " << l.end() << ")";
  return out;
}

template <typename Format>
static
void
object_composite()
{
  std::stringstream stream;
  {
    typename Format::SerializerOut output(stream);
    Line l(Point(42, 51), Point(69, 86));
    l.serialize(output);
  }
  {
    typename Format::SerializerIn input(stream);
    Line l(input);
    BOOST_CHECK_EQUAL(l, Line(Point(42, 51), Point(69, 86)));
  }
}

template <template <typename, typename> class Container>
class Lists
{
public:
  Lists()
    : ints()
    , strings()
  {}

  Lists(elle::serialization::Serializer& s)
    : ints()
    , strings()
  {
    this->serialize(s);
  }

  void
  serialize(elle::serialization::Serializer& s)
  {
    s.serialize("ints", this->ints);
    s.serialize("strings", this->strings);
  }

  Container<int, std::allocator<int>> ints;
  Container<std::string, std::allocator<std::string>> strings;
};

template <typename Format, template <typename, typename> class Container>
static
void
array()
{
  std::stringstream stream;
  {
    typename Format::SerializerOut output(stream);
    Lists<Container> l;
    l.ints = {0, 1, 2};
    l.strings = {"foo", "bar", "baz"};
    l.serialize(output);
  }
  {
    typename Format::SerializerIn input(stream);
    Lists<Container> l(input);
    BOOST_CHECK_EQUAL(l.ints,
                      (Container<int, std::allocator<int>>{0, 1, 2}));
    BOOST_CHECK_EQUAL(l.strings,
                      (Container<std::string, std::allocator<std::string>>
                      {"foo", "bar", "baz"}));
  }
}

template <typename Format>
static
void
pair()
{
  std::stringstream stream;
  {
    typename Format::SerializerOut output(stream);
    std::pair<int, std::string> p(4, "foo");
    output.serialize("pair", p);
  }
  std::cerr << stream.str() << std::endl;
  {
    typename Format::SerializerIn input(stream);
    std::pair<int, std::string> p;
    input.serialize("pair", p);
    BOOST_CHECK_EQUAL(p, (std::pair<int, std::string>(4, "foo")));
  }
}


static
void
json_type_error()
{
  std::stringstream stream(
    "{"
    "  \"int\": true"
    "}"
    );
  typename elle::serialization::json::SerializerIn input(stream);
  int v;
  try
  {
    input.serialize("int", v);
  }
  catch (elle::serialization::TypeError const& e)
  {
    BOOST_CHECK_EQUAL(e.field(), "int");
    BOOST_CHECK(*e.expected() == typeid(int64_t));
    BOOST_CHECK(*e.effective() == typeid(bool));
    return;
  }
  BOOST_FAIL("type error expected");
}

static
void
json_missing_key()
{
  std::stringstream stream(
    "{"
    "  \"a\": 0,"
    "  \"c\": 2"
    "}"
    );
  typename elle::serialization::json::SerializerIn input(stream);
  int v;
  try
  {
    input.serialize("b", v);
  }
  catch (elle::serialization::MissingKey const& e)
  {
    BOOST_CHECK_EQUAL(e.field(), "b");
    return;
  }
  BOOST_FAIL("type error expected");
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(integer<elle::serialization::Json>));
  suite.add(BOOST_TEST_CASE(object<elle::serialization::Json>));
  suite.add(BOOST_TEST_CASE(object_composite<elle::serialization::Json>));
  auto list = &array<elle::serialization::Json, std::list>;
  suite.add(BOOST_TEST_CASE(list));
  auto deque = &array<elle::serialization::Json, std::deque>;
  suite.add(BOOST_TEST_CASE(deque));
  auto vector = &array<elle::serialization::Json, std::vector>;
  suite.add(BOOST_TEST_CASE(vector));
  suite.add(BOOST_TEST_CASE(pair<elle::serialization::Json>));
  suite.add(BOOST_TEST_CASE(json_type_error));
  suite.add(BOOST_TEST_CASE(json_missing_key));
}
