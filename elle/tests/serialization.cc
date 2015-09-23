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
#include <elle/serialization/binary.hh>
#include <elle/serialization/json.hh>
#include <elle/serialization/json/MissingKey.hh>
#include <elle/serialization/json/Overflow.hh>
#include <elle/serialization/json/TypeError.hh>
#include <elle/test.hh>

ELLE_LOG_COMPONENT("elle.serialization.test");

template <typename Format>
static
void
fundamentals()
{
  std::stringstream stream;
  {
    typename Format::SerializerOut output(stream);
    int i = -42;
    output.serialize("int", i);
    unsigned int ui = 42;
    output.serialize("unsigned int", ui);
    double d = 51.51;
    output.serialize("double", d);
    double round = 51.;
    output.serialize("round", round);
    std::vector<int64_t> values({0,1,-1, 63, 64, -63, -64, 65535, 65536,-65535,-65536, 5000000000, -5000000000, 1152921504606846976, 4611686018427387905});
    output.serialize("values", values);
  }
  {
    typename Format::SerializerIn input(stream);
    int i = 0;
    input.serialize("int", i);
    BOOST_CHECK_EQUAL(i, -42);
    int ui = 0;
    input.serialize("unsigned int", ui);
    BOOST_CHECK_EQUAL(ui, 42);
    double d = 0;
    input.serialize("double", d);
    BOOST_CHECK_EQUAL(d, 51.51);
    double round = 0;
    input.serialize("round", round);
    std::vector<int64_t> values({0,1,-1, 63, 64, -63, -64, 65535, 65536,-65535,-65536, 5000000000, -5000000000, 1152921504606846976, 4611686018427387905});
    std::vector<int64_t> ovalues;
    input.serialize("values", ovalues);
    BOOST_CHECK_EQUAL(ovalues, values);
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

  typedef elle::serialization_tag serialization_tag;

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

  typedef elle::serialization_tag serialization_tag;

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
    s.serialize("empty", this->empty);
  }

  Container<int, std::allocator<int>> ints;
  Container<std::string, std::allocator<std::string>> strings;
  Container<int, std::allocator<int>> empty;
};

template <typename Format, template <typename, typename> class Container>
static
void
collection()
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
list()
{
  return collection<Format, std::list>();
}

template <typename Format>
static
void
deque()
{
  return collection<Format, std::deque>();
}

template <typename Format>
static
void
vector()
{
  return collection<Format, std::vector>();
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
  {
    typename Format::SerializerIn input(stream);
    std::pair<int, std::string> p;
    input.serialize("pair", p);
    BOOST_CHECK_EQUAL(p, (std::pair<int, std::string>(4, "foo")));
  }
}

template <typename Format>
static
void
option()
{
  std::stringstream stream;
  {
    boost::optional<int> empty;
    boost::optional<int> filled(42);
    boost::optional<std::vector<int>> collection(std::vector<int>({0, 1, 2}));
    typename Format::SerializerOut output(stream);
    output.serialize("empty", empty);
    output.serialize("filled", filled);
    output.serialize("collection", collection);
  }
  {
    boost::optional<int> empty;
    boost::optional<int> filled;
    boost::optional<std::vector<int>> collection;
    typename Format::SerializerIn input(stream);
    input.serialize("empty", empty);
    input.serialize("filled", filled);
    input.serialize("collection", collection);
    BOOST_CHECK(!empty);
    BOOST_CHECK_EQUAL(filled.get(), 42);
    BOOST_CHECK_EQUAL(collection.get(), std::vector<int>({0, 1, 2}));
  }
}

template <typename Format>
static
void
unique_ptr()
{
  std::stringstream stream;
  {
    std::unique_ptr<int> empty;
    std::unique_ptr<int> filled(new int(42));
    typename Format::SerializerOut output(stream);
    output.serialize("empty", empty);
    output.serialize("filled", filled);
  }
  {
    std::unique_ptr<int> empty;
    std::unique_ptr<int> filled;
    typename Format::SerializerIn input(stream);
    input.serialize("empty", empty);
    input.serialize("filled", filled);
    BOOST_CHECK(!empty);
    BOOST_CHECK_EQUAL(*filled, 42);
  }
}

template <typename Format>
static
void
unordered_map()
{
  std::unordered_map<int, std::string> map{
    {0, "zero"}, {1, "one"}, {2, "two"}};
  std::stringstream stream;
  {
    typename Format::SerializerOut output(stream);
    output.serialize("map", map);
  }
  {
    std::unordered_map<int, std::string> res;
    typename Format::SerializerIn input(stream);
    input.serialize("map", res);
    BOOST_CHECK_EQUAL(map, res);
  }
}

template <typename Format>
static
void
buffer()
{
  elle::Buffer buffer(256);
  for (int i = 0; i < 256; ++i)
    buffer[i] = i;
  std::stringstream stream;
  {
    typename Format::SerializerOut output(stream);
    output.serialize("buffer", buffer);
  }
  {
    elle::Buffer res;
    typename Format::SerializerIn input(stream);
    input.serialize("buffer", res);
    BOOST_CHECK_EQUAL(buffer, res);
  }
}

template <typename Format>
static
void
date()
{
  auto now = boost::posix_time::microsec_clock().local_time();
  std::stringstream stream;
  {
    typename Format::SerializerOut output(stream);
    output.serialize("date", now);
  }
  std::cerr << stream.str() << std::endl;
  {
    boost::posix_time::ptime res;
    typename Format::SerializerIn input(stream);
    input.serialize("date", res);
    BOOST_CHECK_EQUAL(now, res);
  }
}

class Super
  : public elle::serialization::VirtuallySerializable
{
public:
  Super(int i)
    : _i(i)
  {}

  Super(elle::serialization::SerializerIn& s)
  {
    this->serialize(s);
  }

  virtual
  void
  serialize(elle::serialization::Serializer& s)
  {
    s.serialize("super", this->_i);
  }

  virtual
  int
  type()
  {
    return this->_i;
  }

  typedef elle::serialization_tag serialization_tag;

  ELLE_ATTRIBUTE_R(int, i);
};
static const elle::serialization::Hierarchy<Super>::Register<Super> _register_Super;

class Sub1
  : public Super
{
public:
  Sub1(int i)
    : Super(i - 1)
    , _i(i)
  {}

  Sub1(elle::serialization::SerializerIn& s)
    : Super(s)
    , _i(-1)
  {
    this->_serialize(s);
  }

  virtual
  void
  serialize(elle::serialization::Serializer& s) override
  {
    Super::serialize(s);
    this->_serialize(s);
  }

  void
  _serialize(elle::serialization::Serializer& s)
  {
    s.serialize("sub1", this->_i);
  }

  virtual
  int
  type() override
  {
    return this->_i;
  }

  ELLE_ATTRIBUTE_R(int, i);
};
static const elle::serialization::Hierarchy<Super>::Register<Sub1> _register_Sub1;

class Sub2
  : public Super
{
public:
  Sub2(int i)
    : Super(i - 1)
    , _i(i)
  {}

  Sub2(elle::serialization::SerializerIn& s)
    : Super(s)
    , _i(-1)
  {
    this->_serialize(s);
  }

  virtual
  void
  serialize(elle::serialization::Serializer& s) override
  {
    Super::serialize(s);
    this->_serialize(s);
  }

  void
  _serialize(elle::serialization::Serializer& s)
  {
    s.serialize("sub2", this->_i);
  }

  virtual
  int
  type() override
  {
    return this->_i;
  }

  ELLE_ATTRIBUTE_R(int, i);
};
static const elle::serialization::Hierarchy<Super>::Register<Sub2> _register_Sub2;

template <typename Format>
static
void
hierarchy()
{
  std::stringstream stream;
  {
    typename Format::SerializerOut output(stream, false);
    std::unique_ptr<Super> super(new Super(0));
    std::unique_ptr<Super> s1(new Sub1(2));
    std::unique_ptr<Super> s2(new Sub2(3));
    output.serialize("super", super);
    output.serialize("sub1", s1);
    output.serialize("sub2", s2);
  }
  {
    typename Format::SerializerIn output(stream, false);
    std::shared_ptr<Super> ptr;
    output.serialize("super", ptr);
    BOOST_CHECK_EQUAL(ptr->type(), 0);
    output.serialize("sub1", ptr);
    BOOST_CHECK_EQUAL(ptr->type(), 2);
    output.serialize("sub2", ptr);
    BOOST_CHECK_EQUAL(ptr->type(), 3);
  }
}

namespace lib
{
  struct serialization
  {
    static elle::Version version;
  };

  elle::Version serialization::version(0, 3, 0);

  class Versioned
  {
  public:
    typedef serialization serialization_tag;

    Versioned(int old, int addition)
      : _old(old)
      , _addition(addition)
    {}

    Versioned(elle::serialization::Serializer& s)
      : _old(0)
      , _addition(0)
    {
      s.serialize_forward(*this);
    }

    bool
    operator ==(Versioned const& other) const
    {
      return this->old() == other.old() && this->addition() == other.addition();
    }

    void
    serialize(elle::serialization::Serializer& s, elle::Version const& v)
    {
      s.serialize("old", this->_old);
      if (v >= elle::Version(0, 2, 0))
        s.serialize("addition", this->_addition);
    }

    ELLE_ATTRIBUTE_R(int, old);
    ELLE_ATTRIBUTE_R(int, addition);
  };
}

template <typename Format>
static
void
versioning()
{
  ELLE_LOG("test old -> old serialization")
  {
    auto version = elle::scoped_assignment(lib::serialization::version,
                                           elle::Version(0, 1, 0));
    std::stringstream stream;
    {
      typename Format::SerializerOut output(stream);
      lib::Versioned v(1, 2);
      output.serialize_forward(v);
    }
    ELLE_LOG("serialized: %s", stream.str());
    {
      typename Format::SerializerIn input(stream);
      lib::Versioned v(input);
      BOOST_CHECK_EQUAL(v.old(), 1);
      BOOST_CHECK_EQUAL(v.addition(), 0);
    }
  }
  ELLE_LOG("test old -> new serialization")
  {
    std::stringstream stream;
    {
      auto version = elle::scoped_assignment(lib::serialization::version,
                                             elle::Version(0, 1, 0));
      typename Format::SerializerOut output(stream);
      lib::Versioned v(1, 2);
      output.serialize_forward(v);
    }
    ELLE_LOG("serialized: %s", stream.str());
    {
      typename Format::SerializerIn input(stream);
      lib::Versioned v(input);
      BOOST_CHECK_EQUAL(v.old(), 1);
      BOOST_CHECK_EQUAL(v.addition(), 0);
    }
  }
  ELLE_LOG("test new -> new serialization")
  {
    std::stringstream stream;
    {
      typename Format::SerializerOut output(stream);
      lib::Versioned v(1, 2);
      output.serialize_forward(v);
    }
    ELLE_LOG("serialized: %s", stream.str());
    {
      typename Format::SerializerIn input(stream);
      lib::Versioned v(input);
      BOOST_CHECK_EQUAL(v.old(), 1);
      BOOST_CHECK_EQUAL(v.addition(), 2);
    }
  }
}

class InPlace
{
public:
  InPlace(elle::serialization::SerializerIn&)
  {}

  InPlace(InPlace const&) = delete;

  void serialize(elle::serialization::Serializer&)
  {}

  typedef elle::serialization_tag serialization_tag;
};

class OutPlace
{
public:
  OutPlace(OutPlace const&) = delete;

  void serialize(elle::serialization::Serializer&)
  {}

  typedef elle::serialization_tag serialization_tag;
};

static
void
in_place()
{
  std::stringstream stream("{}");
  elle::serialization::json::SerializerIn input(stream);

  std::shared_ptr<InPlace> in;
  std::shared_ptr<InPlace> out;
  input.serialize("in", in);
  input.serialize("out", out);
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

static
void
json_overflows()
{
  std::stringstream stream(
    "{"
    "  \"8_overflow\": 128,"
    "  \"8_noverflow\": 127,"
    "  \"8_underflow\": -129,"
    "  \"8_nunderflow\": -128,"
    "  \"8u_overflow\": 256,"
    "  \"8u_noverflow\": 255,"
    "  \"8u_underflow\": -1,"
    "  \"8u_nunderflow\": 0,"
    "  \"32_overflow\": 2147483648,"
    "  \"32_noverflow\": 2147483647,"
    "  \"32_underflow\": -2147483649,"
    "  \"32_nunderflow\": -2147483648,"
    "  \"32u_overflow\": 4294967296,"
    "  \"32u_noverflow\": 4294967295,"
    "  \"32u_underflow\": -1,"
    "  \"32u_nunderflow\": 0"
    "}"
    );
  typename elle::serialization::json::SerializerIn input(stream);
  int8_t i8;
  uint8_t ui8;
  BOOST_CHECK_THROW(input.serialize("8_overflow", i8),
                    elle::serialization::json::Overflow);
  BOOST_CHECK_THROW(input.serialize("8_underflow", i8),
                    elle::serialization::json::Overflow);
  BOOST_CHECK_THROW(input.serialize("8u_overflow", ui8),
                    elle::serialization::json::Overflow);
  BOOST_CHECK_THROW(input.serialize("8u_underflow", ui8),
                    elle::serialization::json::Overflow);
  input.serialize("8_noverflow", i8);
  input.serialize("8_nunderflow", i8);
  input.serialize("8u_noverflow", ui8);
  input.serialize("8u_nunderflow", ui8);
  int32_t i32;
  uint32_t ui32;
  BOOST_CHECK_THROW(input.serialize("32_overflow", i32),
                    elle::serialization::json::Overflow);
  BOOST_CHECK_THROW(input.serialize("32_underflow", i32),
                    elle::serialization::json::Overflow);
  BOOST_CHECK_THROW(input.serialize("32u_overflow", ui32),
                    elle::serialization::json::Overflow);
  BOOST_CHECK_THROW(input.serialize("32u_underflow", ui32),
                    elle::serialization::json::Overflow);
  input.serialize("32_noverflow", i32);
  input.serialize("32_nunderflow", i32);
  input.serialize("32u_noverflow", ui32);
  input.serialize("32u_nunderflow", ui32);
}

static
void
json_iso8601()
{
  boost::gregorian::date const date(2014, 11, 05);
  boost::posix_time::ptime const expected
    (date, boost::posix_time::seconds((11 * 60 + 36) * 60 + 10));
  // Reading test
  {
    std::stringstream stream(
      "{"
      "  \"date\": \"2014-11-05\","
      "  \"date-time\": \"2014-11-05T11:36:10\","
      "  \"date-time-utc\": \"2014-11-05T11:36:10Z\","
      "  \"date-time-timezone\": \"2014-11-05T12:36:10+0100\","
      "  \"error-garbage\": \"GIGO\","
      "  \"error-garbage-end\": \"2014-11-05T11:36:10+0200GIGO\""
      "}"
      );
    typename elle::serialization::json::SerializerIn input(stream);
    {
      boost::posix_time::ptime time;
      input.serialize("date", time);
      BOOST_CHECK_EQUAL(time, boost::posix_time::ptime(date));
      input.serialize("date-time", time);
      BOOST_CHECK_EQUAL(time, expected);
      input.serialize("date-time-utc", time);
      BOOST_CHECK_EQUAL(time, expected);
      input.serialize("date-time-timezone", time);
      BOOST_CHECK_EQUAL(time, expected);
      BOOST_CHECK_THROW(input.serialize("error-garbage", time),
                        elle::serialization::Error);
      BOOST_CHECK_THROW(input.serialize("error-garbage-end", time),
                        elle::serialization::Error);
    }
  }
  // Writing test
  {
    std::stringstream stream;
    {
      elle::serialization::json::SerializerOut output(stream);
      auto date = expected;
      output.serialize("date", date);
    }
    auto json = elle::json::read(stream);
    auto object = boost::any_cast<elle::json::Object>(json);
    auto str = boost::any_cast<std::string>(object.at("date"));
    BOOST_CHECK_EQUAL(str, "2014-11-05T11:36:10");
  }
}

static
void
json_unicode_surrogate()
{
  // Check we don't choke on a surrogate pair.
  std::stringstream stream(
    "{"
    "    \"foo\": \"\\uD83D\\uDE18\","
    "    \"bar\": \"\\u00E9\\u00E9\""
    "}"
    );
  elle::serialization::json::SerializerIn serializer(stream);
  std::string res;
  serializer.serialize("foo", res);
  BOOST_CHECK_EQUAL(res, "😘");
  serializer.serialize("bar", res);
  BOOST_CHECK_EQUAL(res, "éé");
}

static
void
json_optionals()
{
  boost::optional<std::string> value;
  BOOST_CHECK(!value);
  {
    std::stringstream stream("{}");
    elle::serialization::json::SerializerIn serializer(stream);
    serializer.serialize("value", value);
    BOOST_CHECK(!value);
  }
  {
    std::stringstream stream(
      "{"
      "  \"value\": \"castor\""
      "}");
    elle::serialization::json::SerializerIn serializer(stream);
    serializer.serialize("value", value);
    BOOST_CHECK(value);
    BOOST_CHECK_EQUAL(value.get(), "castor");
  }
  {
    std::stringstream stream(
      "{"
      "}");
    elle::serialization::json::SerializerIn serializer(stream);
    serializer.serialize("value", value);
    BOOST_CHECK(value);
    BOOST_CHECK_EQUAL(value.get(), "castor");
  }
  {
    std::stringstream stream(
      "{"
      "  \"value\": \"polux\""
      "}");
    elle::serialization::json::SerializerIn serializer(stream);
    serializer.serialize("value", value);
    BOOST_CHECK(value);
    BOOST_CHECK_EQUAL(value.get(), "polux");
  }
  {
    std::stringstream stream(
      "{"
      "  \"value\": null"
      "}");
    elle::serialization::json::SerializerIn serializer(stream);
    serializer.serialize("value", value);
    BOOST_CHECK(!value);
  }
}

class Context
{
public:
  Context(std::string message, std::string context)
    : _msg(message)
  {}

  Context(elle::serialization::SerializerIn& input)
  {
    this->serialize(input);
  }

  void
  serialize(elle::serialization::Serializer& s)
  {
    s.serialize_context<std::string>(this->_ctx);
    s.serialize("msg", this->_msg);
  }

  typedef elle::serialization_tag serialization_tag;

  ELLE_ATTRIBUTE_R(std::string, msg);
  ELLE_ATTRIBUTE_R(std::string, ctx);
};

template <typename Format>
static
void
context()
{
  // Not binary friendly
  // {
  //   std::stringstream stream(
  //     "{"
  //     "  \"msg\": \"/r/nocontext\""
  //     "}"
  //     );
  //   typename Format::SerializerIn serializer(stream, false);
  //   serializer.template set_context<std::string>("/r/withcontext");
  //   Context ctx(serializer);
  //   BOOST_CHECK_EQUAL(ctx.msg(), "/r/nocontext");
  //   BOOST_CHECK_EQUAL(ctx.ctx(), "/r/withcontext");
  // }
  {
    Context source("message", "nope");
    std::stringstream stream;
    {
      typename Format::SerializerOut serializer(stream, false);
      serializer.serialize_forward(source);
    }
    typename Format::SerializerIn serializer(stream, false);
    serializer.template set_context<std::string>("yes");
    Context ctx(serializer);
    BOOST_CHECK_EQUAL(ctx.msg(), source.msg());
    BOOST_CHECK_EQUAL(ctx.ctx(), "yes");
  }
}

class SpecificError
  : public elle::Error
{
public:
  SpecificError()
    : elle::Error("much details wow")
    , _dummy(857)
  {}

  SpecificError(elle::serialization::SerializerIn& s)
    : elle::Error("much details wow")
    , _dummy(0)
  {
    this->serialize(s);
  }

  void
  serialize(elle::serialization::Serializer& s)
  {
    s.serialize("dummy", this->_dummy);
  }

  int _dummy;
};
static const elle::serialization::Hierarchy<elle::Exception>::
Register<SpecificError> _register_SpecificError;

template <typename Format>
static
void
exceptions()
{
  std::stringstream stream;
  {
    typename Format::SerializerOut serializer(stream, false);
    std::exception_ptr e;
    try
    {
      throw SpecificError();
    }
    catch (...)
    {
      serializer.serialize_forward(std::current_exception());
    }
  }
  {
    typename Format::SerializerIn serializer(stream, false);
    std::exception_ptr e;
    serializer.serialize_forward(e);
    BOOST_CHECK(e);
    try
    {
      std::rethrow_exception(e);
    }
    catch (SpecificError const& e)
    {
      BOOST_CHECK_EQUAL(e._dummy, 857);
    }
  }
}

#define FOR_ALL_SERIALIZATION_TYPES(Name)                               \
  {                                                                     \
    boost::unit_test::test_suite* subsuite = BOOST_TEST_SUITE(#Name);   \
    auto json = &Name<elle::serialization::Json>;                       \
    subsuite->add(BOOST_TEST_CASE(json));                               \
    auto binary = &Name<elle::serialization::Binary>;                   \
    subsuite->add(BOOST_TEST_CASE(binary));                             \
    suite.add(subsuite);                                                \
  }                                                                     \

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  FOR_ALL_SERIALIZATION_TYPES(fundamentals);
  FOR_ALL_SERIALIZATION_TYPES(object);
  FOR_ALL_SERIALIZATION_TYPES(object_composite);
  FOR_ALL_SERIALIZATION_TYPES(list);
  FOR_ALL_SERIALIZATION_TYPES(deque);
  FOR_ALL_SERIALIZATION_TYPES(vector);
  FOR_ALL_SERIALIZATION_TYPES(pair);
  FOR_ALL_SERIALIZATION_TYPES(option);
  FOR_ALL_SERIALIZATION_TYPES(unique_ptr);
  FOR_ALL_SERIALIZATION_TYPES(unordered_map);
  FOR_ALL_SERIALIZATION_TYPES(buffer);
  FOR_ALL_SERIALIZATION_TYPES(date);
  FOR_ALL_SERIALIZATION_TYPES(hierarchy);
  FOR_ALL_SERIALIZATION_TYPES(versioning);
  FOR_ALL_SERIALIZATION_TYPES(context);
  FOR_ALL_SERIALIZATION_TYPES(exceptions);
  suite.add(BOOST_TEST_CASE(in_place));
  suite.add(BOOST_TEST_CASE(json_type_error));
  suite.add(BOOST_TEST_CASE(json_missing_key));
  suite.add(BOOST_TEST_CASE(json_overflows));
  suite.add(BOOST_TEST_CASE(json_iso8601));
  suite.add(BOOST_TEST_CASE(json_unicode_surrogate));
  suite.add(BOOST_TEST_CASE(json_optionals));
}
