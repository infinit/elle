#include <deque>
#include <list>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include <elle/attribute.hh>
#include <elle/filesystem/path.hh>
#include <elle/serialization/binary.hh>
#include <elle/serialization/json.hh>
#include <elle/serialization/json/Error.hh>
#include <elle/test.hh>

ELLE_LOG_COMPONENT("elle.serialization.test");

class InPlace
{
public:
  InPlace(int i)
  {}

  InPlace(elle::serialization::SerializerIn&)
  {}

  InPlace(InPlace const&) = delete;
  InPlace(InPlace&&) = default;

  bool
  operator == (InPlace const& other) const
  {
    return this == &other;
  }

  void serialize(elle::serialization::Serializer&)
  {}

  using serialization_tag = elle::serialization_tag;
};

class OutPlace
{
public:
  OutPlace() = default;
  OutPlace(OutPlace const&) = delete;
  OutPlace(OutPlace&&) = default;

  void serialize(elle::serialization::Serializer&)
  {}

  using serialization_tag = elle::serialization_tag;
};

namespace std
{
  template <>
  struct hash<InPlace>
  {
    std::size_t
    operator ()(InPlace const& ip) const
    {
      return std::hash<void const*>()(&ip);
    }
  };
}

template <typename Format>
static
void
fundamentals()
{
  auto const values =
    std::vector<int64_t>{0,
                         1, -1,
                         63, 64, -63, -64,
                         65535, 65536, -65535, -65536,
                         5000000000, -5000000000,
                         1152921504606846976, 4611686018427387905};
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
    std::vector<int64_t> ovalues;
    input.serialize("values", ovalues);
    BOOST_TEST(ovalues == values);
  }
}

class Point
{
public:
  Point(int x = 0, int y = 0)
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

  using serialization_tag = elle::serialization_tag;

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

class Segment
{
public:
  Segment()
    : _start()
    , _end()
  {}

  Segment(Point start, Point end)
    : _start(start)
    , _end(end)
  {}

  Segment(elle::serialization::Serializer& s)
    : Segment()
  {
    this->serialize(s);
  }

  bool
  operator ==(Segment const& other) const
  {
    return this->start() == other.start() && this->end() == other.end();
  }

  void
  serialize(elle::serialization::Serializer& s)
  {
    s.serialize("start", this->_start);
    s.serialize("end", this->_end);
  }

  using serialization_tag = elle::serialization_tag;

  ELLE_ATTRIBUTE_R(Point, start);
  ELLE_ATTRIBUTE_R(Point, end);
};

static
std::ostream&
operator << (std::ostream& out, Segment const& l)
{
  out << "Segment(" << l.start() << ", " << l.end() << ")";
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
    Segment l(Point(42, 51), Point(69, 86));
    l.serialize(output);
  }
  {
    typename Format::SerializerIn input(stream);
    Segment l(input);
    BOOST_CHECK_EQUAL(l, Segment(Point(42, 51), Point(69, 86)));
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
    s.serialize("options", this->options);
  }

  Container<int, std::allocator<int>> ints;
  Container<std::string, std::allocator<std::string>> strings;
  Container<int, std::allocator<int>> empty;
  Container<boost::optional<int>, std::allocator<boost::optional<int>>> options;
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
    l.options = {{42}, {}};
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
    BOOST_CHECK(l.empty.empty());
    BOOST_CHECK_EQUAL(
      l.options,
      (Container<boost::optional<int>, std::allocator<boost::optional<int>>>
      {{42}, {}}));
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
    boost::optional<Point> object(Point(1, 2));
    boost::optional<std::vector<int>> collection(std::vector<int>({0, 1, 2}));
    typename Format::SerializerOut output(stream);
    output.serialize("empty", empty);
    output.serialize("filled", filled);
    output.serialize("object", object);
    output.serialize("collection", collection);
  }
  {
    boost::optional<int> empty;
    boost::optional<int> filled;
    boost::optional<Point> object;
    boost::optional<std::vector<int>> collection;
    typename Format::SerializerIn input(stream);
    input.serialize("empty", empty);
    input.serialize("filled", filled);
    input.serialize("object", object);
    input.serialize("collection", collection);
    BOOST_CHECK(!empty);
    BOOST_REQUIRE(filled);
    BOOST_CHECK_EQUAL(filled.get(), 42);
    BOOST_REQUIRE(object);
    BOOST_CHECK_EQUAL(object->x(), 1);
    BOOST_CHECK_EQUAL(object->y(), 2);
    BOOST_REQUIRE(collection);
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
    std::unique_ptr<Point> object(new Point(1, 2));
    typename Format::SerializerOut output(stream);
    output.serialize("empty", empty);
    output.serialize("filled", filled);
    output.serialize("object", object);
  }
  ELLE_LOG("serialized: %s", elle::ConstWeakBuffer(stream.str()));
  {
    std::unique_ptr<int> empty;
    std::unique_ptr<int> filled;
    std::unique_ptr<Point> object;
    typename Format::SerializerIn input(stream);
    input.serialize("empty", empty);
    input.serialize("filled", filled);
    input.serialize("object", object);
    BOOST_CHECK(!empty);
    BOOST_REQUIRE(filled);
    BOOST_CHECK_EQUAL(*filled, 42);
    BOOST_REQUIRE(object);
    BOOST_CHECK_EQUAL(object->x(), 1);
    BOOST_CHECK_EQUAL(object->y(), 2);
  }
  {
    std::unique_ptr<int, std::function<void (int*)>> p(
      new int(42), [] (int* p) { delete p; });
    std::stringstream ss;
    elle::serialization::serialize<Format>(p, ss);
    typename Format::SerializerIn input(ss);
    input.serialize_forward(p);
  }
}

template <typename Format>
static
void
raw_ptr()
{
  std::stringstream stream;
  {
    int* empty = nullptr;
    int* filled(new int(42));
    typename Format::SerializerOut output(stream);
    output.serialize("empty", empty);
    output.serialize("filled", filled);
    delete filled;
  }
  {
    int* empty = reinterpret_cast<int*>(0x42);
    int* filled = nullptr;
    typename Format::SerializerIn input(stream);
    input.serialize("empty", empty);
    input.serialize("filled", filled);
    BOOST_CHECK(!empty);
    BOOST_CHECK(filled);
    BOOST_CHECK_EQUAL(*filled, 42);
    delete filled;
  }
}


template <typename Format>
static
void
unordered_map()
{
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
  {
    using Map = std::unordered_map<InPlace, InPlace>;
    Map map;
    map.emplace(0, 1);
    map.emplace(2, 3);
    auto ser = elle::serialization::serialize<Format>(map);
    auto deser = elle::serialization::deserialize<Format, Map>(ser);
  }
}

template <typename Format>
static
void
unordered_map_string()
{
  std::unordered_map<std::string, int> map
  {
    {"zero", 0}, {"one", 1}, {"two", 2}
  };
  std::stringstream stream;
  {
    typename Format::SerializerOut output(stream);
    output.serialize("map", map);
  }
  ELLE_DUMP("out: %s", stream.str());
  {
    std::unordered_map<std::string, int> res;
    typename Format::SerializerIn input(stream);
    input.serialize("map", res);
    BOOST_CHECK_EQUAL(map, res);
    ELLE_DUMP("in:");
    for (auto const& elt: res)
      ELLE_DUMP("%s, %s", elt.first, elt.second);
  }
}

template <typename Format>
static
void
unordered_set()
{
  auto set = std::unordered_set<int>{0, 1, 2};
  std::stringstream stream;
  {
    typename Format::SerializerOut output(stream);
    output.serialize("set", set);
  }
  {
    std::unordered_set<int> res;
    typename Format::SerializerIn input(stream);
    input.serialize("set", res);
    BOOST_CHECK_EQUAL(set, res);
  }
}

static
void
unordered_map_string_legacy()
{
  using Map = std::unordered_map<std::string, std::string>;
  auto reference = Map
    {
      {"screenshot_modal", "v1"},
      {"send_view_20141027", "a"},
      {"drip_onboarding_template", "a"},
      {"send_file_url_template", "send-file-url"},
      {"screenshot_modal_20141104", "b"},
      {"drip_ghost-reminder_template", "control"},
      {"drip_delight-sender_template", "a"},
    };
  std::stringstream stream;
  stream << "{\"features\":[\
                [\"screenshot_modal\",\"v1\"],\
                [\"send_view_20141027\",\"a\"],\
                [\"drip_onboarding_template\",\"a\"],\
                [\"send_file_url_template\",\"send-file-url\"],\
                [\"screenshot_modal_20141104\",\"b\"],\
                [\"drip_ghost-reminder_template\",\"control\"],\
                [\"drip_delight-sender_template\",\"a\"]\
                ],}";
  auto input = elle::serialization::json::SerializerIn{stream};
  BOOST_CHECK_EQUAL(reference,
                    input.deserialize<Map>("features"));
}

template <typename Format>
static
void
buffer()
{
  auto buffer = elle::Buffer(256);
  for (int i = 0; i < 256; ++i)
    buffer[i] = i;
  std::stringstream stream;
  {
    auto output = typename Format::SerializerOut{stream};
    output.serialize("buffer", buffer);
  }
  {
    auto res = elle::Buffer{};
    auto input = typename Format::SerializerIn{stream};
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
  {
    boost::posix_time::ptime res;
    typename Format::SerializerIn input(stream);
    input.serialize("date", res);
    BOOST_CHECK_EQUAL(now, res);
  }
}

template <typename Format>
static
void
version()
{
  std::stringstream stream;
  auto version = elle::Version{2, 27, 123};
  {
    typename Format::SerializerOut output(stream);
    output.serialize("version", version);
  }
  {
    elle::Version res;
    typename Format::SerializerIn input(stream);
    input.serialize("version", res);
    BOOST_CHECK_EQUAL(version, res);
  }
}

template <typename Format, typename Repr, typename Ratio>
static
void
chrono_check(std::chrono::duration<Repr, Ratio> const& d)
{
  std::stringstream stream;
  {
    typename Format::SerializerOut output(stream);
    output.serialize("duration", d);
  }
  {
    std::chrono::duration<Repr, Ratio> res;
    typename Format::SerializerIn input(stream);
    input.serialize("duration", res);
    BOOST_CHECK_EQUAL(d, res);
  }
}

template <typename Format>
static
void
chrono()
{
  chrono_check<Format>(std::chrono::system_clock::duration(601));
  chrono_check<Format>(std::chrono::high_resolution_clock::duration(602));
  chrono_check<Format>(std::chrono::nanoseconds(603));
  chrono_check<Format>(std::chrono::microseconds(604));
  chrono_check<Format>(std::chrono::milliseconds(605));
  chrono_check<Format>(std::chrono::seconds(606));
  chrono_check<Format>(std::chrono::minutes(607));
  chrono_check<Format>(std::chrono::hours(608));
  chrono_check<Format>(std::chrono::hours(609 * 24));
}

template <typename Format>
static
void
path_check(boost::filesystem::path const& path)
{
  std::stringstream stream;
  {
    typename Format::SerializerOut output(stream);
    output.serialize("path", path);
  }
  {
    boost::filesystem::path res;
    typename Format::SerializerIn input(stream);
    input.serialize("path", res);
    BOOST_CHECK(path == res);
  }
}

template <typename Format>
static
void
path()
{
  path_check<Format>("/tmp/elle");
  path_check<Format>("../..");
  path_check<Format>("./elle");
  path_check<Format>(".");
}

template <bool Versioned>
class Super
  : public elle::serialization::VirtuallySerializable<Super<Versioned>,
                                                      Versioned>
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

  using serialization_tag = elle::serialization_tag;

  ELLE_ATTRIBUTE_R(int, i);
};
static const elle::serialization::Hierarchy<Super<false>>::Register<Super<false>>
_register_SuperU("SuperU");

template <>
class Super<true>
  : public elle::serialization::VirtuallySerializable<Super<true>, true>
{
public:
  Super(int i)
    : _i(i)
  {}

  Super(elle::serialization::SerializerIn& s, elle::Version const& v)
  {
    this->serialize(s, v);
  }

  virtual
  void
  serialize(elle::serialization::Serializer& s, elle::Version const&)
  {
    s.serialize("super", this->_i);
  }

  virtual
  int
  type()
  {
    return this->_i;
  }

  using serialization_tag = elle::serialization_tag;

  ELLE_ATTRIBUTE_R(int, i);
};
static const elle::serialization::Hierarchy<Super<true>>::Register<Super<true>>
_register_SuperV("SuperV");

template <bool Versioned>
class Sub1
  : public Super<Versioned>
{
public:
  Sub1(int i)
    : Super<Versioned>(i - 1)
    , _i(i)
  {}

  Sub1(elle::serialization::SerializerIn& s)
    : Super<Versioned>(s)
    , _i(-1)
  {
    this->_serialize(s);
  }

  virtual
  void
  serialize(elle::serialization::Serializer& s) override
  {
    Super<Versioned>::serialize(s);
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
static const elle::serialization::Hierarchy<Super<false>>::Register<Sub1<false>>
_register_Sub1U("Sub1U");

template <>
class Sub1<true>
  : public Super<true>
{
public:
  Sub1(int i)
    : Super<true>(i - 1)
    , _i(i)
  {}

  Sub1(elle::serialization::SerializerIn& s, elle::Version const& v)
    : Super<true>(s, v)
    , _i(-1)
  {
    this->_serialize(s);
  }

  virtual
  void
  serialize(elle::serialization::Serializer& s, elle::Version const& v) override
  {
    Super<true>::serialize(s, v);
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
static const elle::serialization::Hierarchy<Super<true>>::Register<Sub1<true>>
_register_Sub1V("Sub1V");

template <bool Versioned>
class Sub2
  : public Super<Versioned>
{
public:
  Sub2(int i)
    : Super<Versioned>(i - 1)
    , _i(i)
  {}

  Sub2(elle::serialization::SerializerIn& s)
    : Super<Versioned>(s)
    , _i(-1)
  {
    this->_serialize(s);
  }

  virtual
  void
  serialize(elle::serialization::Serializer& s) override
  {
    Super<Versioned>::serialize(s);
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
static const elle::serialization::Hierarchy<Super<false>>::Register<Sub2<false>>
_register_Sub2U("Sub2U");

template <>
class Sub2<true>
  : public Super<true>
{
public:
  Sub2(int i)
    : Super<true>(i - 1)
    , _i(i)
  {}

  Sub2(elle::serialization::SerializerIn& s, elle::Version const& v)
    : Super<true>(s, v)
    , _i(-1)
  {
    this->_serialize(s);
  }

  virtual
  void
  serialize(elle::serialization::Serializer& s, elle::Version const& v) override
  {
    Super<true>::serialize(s, v);
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
static const elle::serialization::Hierarchy<Super<true>>::Register<Sub2<true>>
_register_Sub2V("Sub2V");

template <typename Format, bool Versioned>
static
void
hierarchy()
{
  std::stringstream stream;
  {
    typename Format::SerializerOut output(stream, false);
    std::unique_ptr<Super<Versioned>> super(new Super<Versioned>(0));
    std::unique_ptr<Super<Versioned>> s1(new Sub1<Versioned>(2));
    std::unique_ptr<Sub2<Versioned>> s2(new Sub2<Versioned>(3));
    output.serialize("super", super);
    output.serialize("sub1", s1);
    output.serialize("sub2", s2);
  }
  {
    typename Format::SerializerIn output(stream, false);
    std::shared_ptr<Super<Versioned>> ptr;
    output.serialize("super", ptr);
    BOOST_CHECK_EQUAL(ptr->type(), 0);
    output.serialize("sub1", ptr);
    BOOST_CHECK_EQUAL(ptr->type(), 2);
    output.serialize("sub2", ptr);
    BOOST_CHECK_EQUAL(ptr->type(), 3);
  }
}

namespace versioning
{
  using elle::Version;
  using elle::type_info;
  namespace lib
  {
    struct serialization
    {
      static Version version;
    };

    Version serialization::version(0, 3, 0);

    class Versioned
    {
    public:
      using serialization_tag = serialization;

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
        return this->old() == other.old() &&
          this->addition() == other.addition();
      }

      void
      serialize(elle::serialization::Serializer& s, Version const& v)
      {
        s.serialize("old", this->_old);
        if (v >= Version(0, 2, 0))
          s.serialize("addition", this->_addition);
      }

      ELLE_ATTRIBUTE_R(int, old);
      ELLE_ATTRIBUTE_R(int, addition);
    };
  }

  // static
  // std::ostream&
  // operator <<(std::ostream& o, lib::Versioned const& v)
  // {
  //   elle::fprintf(o, "Versioned(%s, %s)", v.old(), v.addition());
  //   return o;
  // }

  template <typename Format>
  static
  void
  global()
  {
    ELLE_LOG("test old -> old serialization")
    {
      auto version = elle::scoped_assignment(lib::serialization::version,
                                             Version(0, 1, 0));
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
                                               Version(0, 1, 0));
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

  template <typename Format>
  static
  void
  local()
  {
    ELLE_LOG("test old -> old serialization")
    {
      std::unordered_map<elle::TypeInfo, Version> versions;
      versions.emplace(type_info<lib::serialization>(), Version(0, 1, 0));
      std::stringstream stream;
      {
        typename Format::SerializerOut output(stream, versions);
        lib::Versioned v(1, 2);
        output.serialize_forward(v);
      }
      ELLE_LOG("serialized: %s", stream.str());
      {
        typename Format::SerializerIn input(stream, versions);
        lib::Versioned v(input);
        BOOST_CHECK_EQUAL(v.old(), 1);
        BOOST_CHECK_EQUAL(v.addition(), 0);
      }
    }
    ELLE_LOG("test old -> new serialization")
    {
      std::stringstream stream;
      {
        std::unordered_map<elle::TypeInfo, Version> versions;
        versions.emplace(type_info<lib::serialization>(), Version(0, 1, 0));
        typename Format::SerializerOut output(stream, versions);
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

  struct serialization
  {
    static Version version;
    static std::unordered_map<
      Version, elle::serialization::Serializer::Versions> dependencies;
  };
  Version serialization::version(0, 2, 0);
  std::unordered_map<Version, elle::serialization::Serializer::Versions>
    serialization::dependencies
    {
      {Version(0, 2, 0), {{type_info<lib::serialization>(), Version(0, 3, 0)}}},
      {Version(0, 1, 0), {{type_info<lib::serialization>(), Version(0, 1, 0)}}},
    };


  class Owner
  {
  public:
    using serialization_tag = serialization;

    Owner(int i, lib::Versioned const& v)
      : _i(i)
      , _v(std::move(v))
    {}

    Owner(elle::serialization::Serializer& s, elle::Version const& v)
      : _i(0)
      , _v(0, 0)
    {
      this->serialize(s, v);
    }

    bool
    operator ==(Owner const& other) const
    {
      return this->i() == other.i() &&
        this->v() == other.v();
    }

    void
    serialize(elle::serialization::Serializer& s, Version const& v)
    {
      s.serialize("i", this->_i);
      s.serialize("v", this->_v);
    }

    ELLE_ATTRIBUTE_R(int, i);
    ELLE_ATTRIBUTE_R(lib::Versioned, v);
  };

  // static
  // std::ostream&
  // operator <<(std::ostream& o, Owner const& owner)
  // {
  //   elle::fprintf(o, "Owner(%s, %s)", owner.i(), owner.v());
  //   return o;
  // }

  template <typename Format>
  static
  void
  dependencies()
  {
    std::unordered_map<elle::TypeInfo, Version> versions;
    versions.emplace(type_info<lib::serialization>(), Version(0, 1, 0));
    ELLE_LOG("test old -> old serialization versioned")
    {
      Owner o(42, {1, 2});
      auto buffer =
        elle::serialization::serialize<Format>(o, Version(0, 1, 0));
      ELLE_LOG("serialized: %s", buffer);
      auto v = elle::serialization::deserialize<Format, Owner>(buffer);
      BOOST_CHECK_EQUAL(v.i(), 42);
      BOOST_CHECK_EQUAL(v.v().old(), 1);
      BOOST_CHECK_EQUAL(v.v().addition(), 0);
    }
    ELLE_LOG("test old -> old serialization unversioned")
    {
      Owner o(42, {1, 2});
      auto buffer = elle::serialization::serialize<Format>(
        o, Version(0, 1, 0), false);
      ELLE_LOG("serialized: %s", buffer);
      auto v = elle::serialization::deserialize<Format, Owner>(
        buffer, Version(0, 1, 0), false);
      BOOST_CHECK_EQUAL(v.i(), 42);
      BOOST_CHECK_EQUAL(v.v().old(), 1);
      BOOST_CHECK_EQUAL(v.v().addition(), 0);
    }
    ELLE_LOG("test new -> new serialization versioned")
    {
      Owner o(42, {1, 2});
      auto buffer = elle::serialization::serialize<Format>(
        o, Version(0, 2, 0));
      ELLE_LOG("serialized: %s", buffer);
      auto v = elle::serialization::deserialize<Format, Owner>(buffer);
      BOOST_CHECK_EQUAL(v.i(), 42);
      BOOST_CHECK_EQUAL(v.v().old(), 1);
      BOOST_CHECK_EQUAL(v.v().addition(), 2);
    }
    ELLE_LOG("test new -> new serialization unversioned")
    {
      Owner o(42, {1, 2});
      auto buffer = elle::serialization::serialize<Format>(
        o, Version(0, 2, 0), false);
      ELLE_LOG("serialized: %s", buffer);
      auto v = elle::serialization::deserialize<Format, Owner>(
        buffer, Version(0, 2, 0), false);
      BOOST_CHECK_EQUAL(v.i(), 42);
      BOOST_CHECK_EQUAL(v.v().old(), 1);
      BOOST_CHECK_EQUAL(v.v().addition(), 2);
    }
  }
}

static
void
in_place()
{
  std::stringstream stream("{}");
  auto input = elle::serialization::json::SerializerIn(stream);

  std::shared_ptr<InPlace> in;
  std::shared_ptr<OutPlace> out;
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
  auto input = elle::serialization::json::SerializerIn(stream);
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
  auto input = elle::serialization::json::SerializerIn(stream);
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

template <typename T>
void
check_min_max(elle::serialization::json::SerializerIn& i,
              std::string const& name)
{
  auto v = T{};
  i.serialize(name + "_min", v);
  BOOST_TEST(v == std::numeric_limits<T>::min());
  i.serialize(name + "_max", v);
  BOOST_TEST(v == std::numeric_limits<T>::max());
}

template <typename T>
using lim = std::numeric_limits<T>;

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
    "  \"32u_nunderflow\": 0,"
    "  \"64u_min\": " + std::to_string(lim<uint64_t>::min()) + ","
    "  \"64u_max\": " + std::to_string(lim<uint64_t>::max()) + ","
    "}"
    );
  auto input = typename elle::serialization::json::SerializerIn(stream);
  using Overflow = elle::serialization::json::Overflow;
  int8_t i8;
  uint8_t ui8;
  BOOST_CHECK_THROW(input.serialize("8_overflow", i8), Overflow);
  BOOST_CHECK_THROW(input.serialize("8_underflow", i8), Overflow);
  BOOST_CHECK_THROW(input.serialize("8u_overflow", ui8), Overflow);
  BOOST_CHECK_THROW(input.serialize("8u_underflow", ui8), Overflow);
  input.serialize("8_noverflow", i8);
  input.serialize("8_nunderflow", i8);
  input.serialize("8u_noverflow", ui8);
  input.serialize("8u_nunderflow", ui8);
  int32_t i32;
  uint32_t ui32;
  BOOST_CHECK_THROW(input.serialize("32_overflow", i32), Overflow);
  BOOST_CHECK_THROW(input.serialize("32_underflow", i32), Overflow);
  BOOST_CHECK_THROW(input.serialize("32u_overflow", ui32), Overflow);
  BOOST_CHECK_THROW(input.serialize("32u_underflow", ui32), Overflow);

  input.serialize("32_noverflow", i32);
  BOOST_TEST(i32 == 2147483647);
  input.serialize("32_nunderflow", i32);
  BOOST_TEST(i32 == -2147483648);

  input.serialize("32u_noverflow", ui32);
  BOOST_TEST(ui32 == 4294967295);
  input.serialize("32u_nunderflow", ui32);
  BOOST_TEST(ui32 == 0);

  check_min_max<uint64_t>(input, "64u");
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
    auto input = elle::serialization::json::SerializerIn(stream);
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
    //BOOST_CHECK_EQUAL(str, "2014-11-05T11:36:10");
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
  ELLE_LOG("deserialize missing option by ref")
  {
    std::stringstream stream("{}");
    elle::serialization::json::SerializerIn serializer(stream);
    serializer.serialize("value", value);
    BOOST_CHECK(!value);
  }
  ELLE_LOG("deserialize missing option by value")
  {
    std::stringstream stream("{}");
    BOOST_CHECK(!
      elle::serialization::json::deserialize<boost::optional<std::string>>(
        stream, "value"));
  }
  ELLE_LOG("deserialize filled option by ref")
  {
    std::stringstream stream(
      "{"
      "  \"value\": \"castor\""
      "}");
    elle::serialization::json::SerializerIn serializer(stream);
    serializer.serialize("value", value);
    BOOST_REQUIRE(value);
    BOOST_CHECK_EQUAL(value.get(), "castor");
  }
  ELLE_LOG("deserialize missing option in place by ref")
  {
    std::stringstream stream("{}");
    elle::serialization::json::SerializerIn serializer(stream);
    serializer.serialize("value", value);
    BOOST_CHECK(!value);
  }
  ELLE_LOG("deserialize filled option in place by ref")
  {
    std::stringstream stream(
      "{"
      "  \"value\": \"polux\""
      "}");
    elle::serialization::json::SerializerIn serializer(stream);
    serializer.serialize("value", value);
    BOOST_REQUIRE(value);
    BOOST_CHECK_EQUAL(value.get(), "polux");
  }
  ELLE_LOG("deserialize null option in place by ref")
  {
    std::stringstream stream(
      "{"
      "  \"value\": null"
      "}");
    elle::serialization::json::SerializerIn serializer(stream);
    serializer.serialize("value", value);
    BOOST_CHECK(!value);
  }
  ELLE_LOG("serialize null option")
  {
    std::stringstream stream;
    {
      elle::serialization::json::SerializerOut serializer(stream, false);
      boost::optional<bool> o;
      serializer.serialize("value", o);
    }
    auto json = elle::json::read(stream);
    BOOST_CHECK_EQUAL(boost::any_cast<elle::json::Object&>(json).size(), 0);
  }
}

template <typename Format>
static
void
text_parser()
{
  std::stringstream stream;
  {
    typename Format::SerializerOut ser(stream);
    BOOST_CHECK_EQUAL(ser.text(),
                      (std::is_same<Format, elle::serialization::Json>::value));
  }
  {
    typename Format::SerializerIn ser(stream);
    BOOST_CHECK_EQUAL(ser.text(),
                      (std::is_same<Format, elle::serialization::Json>::value));
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

  using serialization_tag = elle::serialization_tag;

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

  SpecificError(elle::serialization::SerializerIn& s,
                elle::Version const& v)
    : elle::Error("much details wow")
    , _dummy(0)
  {
    this->serialize(s, v);
  }

  virtual
  void
  serialize(elle::serialization::Serializer& s,
            elle::Version const&) override
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

class Convertible
{
public:
  Convertible(int64_t i)
    : _i(i)
  {}
  ELLE_ATTRIBUTE_R(int64_t, i);
};

struct PConvertible
{
  int64_t i;
};

namespace elle
{
  namespace serialization
  {
    template <>
    struct Serialize<Convertible>
    {
      using Type = int64_t;

      static
      int64_t
      convert(Convertible const& c)
      {
        return c.i();
      }

      static
      Convertible
      convert(int64_t const& i)
      {
        return Convertible(i);
      }
    };

    // Check one can override even pointer serialization. Think BIGNUM*.
    template <>
    struct Serialize<PConvertible*>
    {
      using Type = int64_t;

      static
      int64_t
      convert(PConvertible* c)
      {
        return c->i;
      }

      static
      PConvertible*
      convert(int64_t i)
      {
        return new PConvertible{i};
      }
    };
  }
}

template <typename Format>
static
void
convert()
{
  Convertible c{1332};
  auto pc = std::unique_ptr<PConvertible>(new PConvertible{1332});
  std::stringstream stream;
  {
    typename Format::SerializerOut serializer(stream, false);
    serializer.serialize("convertible", c);
    serializer.serialize("convertible_value", c);
    serializer.serialize("pconvertible", pc.get());
  }
  {
    typename Format::SerializerIn serializer(stream, false);
    auto r = serializer.template deserialize<Convertible>("convertible");
    BOOST_CHECK_EQUAL(r.i(), c.i());
    Convertible rv{1641};
    serializer.serialize("convertible_value", rv);
    BOOST_CHECK_EQUAL(rv.i(), c.i());
    std::unique_ptr<PConvertible> pr(
      serializer.template deserialize<PConvertible*>("pconvertible"));
    BOOST_CHECK_EQUAL(pr->i, pc->i);
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
  auto& master = boost::unit_test::framework::master_test_suite();
  auto& suite = master;
  FOR_ALL_SERIALIZATION_TYPES(fundamentals);
  FOR_ALL_SERIALIZATION_TYPES(object);
  FOR_ALL_SERIALIZATION_TYPES(object_composite);
  FOR_ALL_SERIALIZATION_TYPES(list);
  FOR_ALL_SERIALIZATION_TYPES(deque);
  FOR_ALL_SERIALIZATION_TYPES(vector);
  FOR_ALL_SERIALIZATION_TYPES(pair);
  FOR_ALL_SERIALIZATION_TYPES(option);
  FOR_ALL_SERIALIZATION_TYPES(unique_ptr);
  FOR_ALL_SERIALIZATION_TYPES(raw_ptr);
  FOR_ALL_SERIALIZATION_TYPES(unordered_map);
  FOR_ALL_SERIALIZATION_TYPES(unordered_map_string);
  FOR_ALL_SERIALIZATION_TYPES(unordered_set);
  FOR_ALL_SERIALIZATION_TYPES(buffer);
  FOR_ALL_SERIALIZATION_TYPES(date);
  FOR_ALL_SERIALIZATION_TYPES(version);
  FOR_ALL_SERIALIZATION_TYPES(chrono);
  FOR_ALL_SERIALIZATION_TYPES(path);
  {
    boost::unit_test::test_suite* subsuite = BOOST_TEST_SUITE("hierarchy");
    {
      boost::unit_test::test_suite* s = BOOST_TEST_SUITE("unversioned");
      auto json = &hierarchy<elle::serialization::Json, false>;
      s->add(BOOST_TEST_CASE(json));
      auto binary = &hierarchy<elle::serialization::Binary, false>;
      s->add(BOOST_TEST_CASE(binary));
      subsuite->add(s);
    }
    {
      boost::unit_test::test_suite* s = BOOST_TEST_SUITE("versioned");
      auto json = &hierarchy<elle::serialization::Json, true>;
      s->add(BOOST_TEST_CASE(json));
      auto binary = &hierarchy<elle::serialization::Binary, true>;
      s->add(BOOST_TEST_CASE(binary));
      subsuite->add(s);
    }
    suite.add(subsuite);
  }
  {
    using namespace versioning;
    auto subsuite = BOOST_TEST_SUITE("versioning");
    master.add(subsuite);
    auto& suite = *subsuite;
    FOR_ALL_SERIALIZATION_TYPES(global);
    FOR_ALL_SERIALIZATION_TYPES(local);
    FOR_ALL_SERIALIZATION_TYPES(dependencies);
  }
  FOR_ALL_SERIALIZATION_TYPES(context);
  FOR_ALL_SERIALIZATION_TYPES(exceptions);
  FOR_ALL_SERIALIZATION_TYPES(text_parser);
  FOR_ALL_SERIALIZATION_TYPES(convert);
  suite.add(BOOST_TEST_CASE(in_place));
  suite.add(BOOST_TEST_CASE(unordered_map_string_legacy));
  suite.add(BOOST_TEST_CASE(json_type_error));
  suite.add(BOOST_TEST_CASE(json_missing_key));
  suite.add(BOOST_TEST_CASE(json_overflows));
  suite.add(BOOST_TEST_CASE(json_iso8601));
  suite.add(BOOST_TEST_CASE(json_unicode_surrogate));
  suite.add(BOOST_TEST_CASE(json_optionals));
}
