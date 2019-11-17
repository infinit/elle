#include <elle/Table.hh>

#include <string>

#include <boost/range/irange.hpp>

#include <elle/log.hh>
#include <elle/range.hh>
#include <elle/serialization/binary.hh>
#include <elle/serialization/json.hh>
#include <elle/test.hh>

ELLE_LOG_COMPONENT("elle.Table.test");

static
void
orientation()
{
  auto constexpr w = 5;
  auto constexpr h = 3;
  elle::Table<bool, 2> t(w, h);
  t.at(0, 0) = true;
  t.at(0, 2) = true;
  t.at(4, 0) = true;
  t.at(4, 2) = true;
}

static
void
pod()
{
  auto constexpr w = 3;
  auto constexpr h = 2;
  elle::Table<int, 2> table(w, h);
  for (auto i : boost::irange(0, w))
    for (auto j : boost::irange(0, h))
      BOOST_TEST((table.at(i, j) = w * i + j) == w * i + j);
  for (auto i : boost::irange(0, w))
    for (auto j : boost::irange(0, h))
      BOOST_TEST(table.at(i, j) == w * i + j);
}

static
void
object()
{
  auto constexpr w = 3;
  auto constexpr h = 2;
  elle::Table<std::string, 2> table(w, h);
  for (auto i : boost::irange(0, w))
    for (auto j : boost::irange(0, h))
      BOOST_TEST(table.at(i, j) == "");
  for (auto i : boost::irange(0, w))
    for (auto j : boost::irange(0, h))
      BOOST_TEST((table.at(i, j) = std::to_string(w * i + j)) ==
                 std::to_string(w * i + j));
  for (auto i : boost::irange(0, w))
    for (auto j : boost::irange(0, h))
      BOOST_TEST(table.at(i, j) == std::to_string(w * i + j));
}

static
void
bounds()
{
  auto constexpr w = 3;
  auto constexpr h = 2;
  elle::Table<int, 2> table(w, h);
  for (auto i : boost::irange(0, w))
  {
    BOOST_TEST(!table.contains({i, h}));
    BOOST_CHECK_THROW(table.at(i, h), elle::Error);
  }
  for (auto j : boost::irange(0, h))
  {
    BOOST_TEST(!table.contains({w, j}));
    BOOST_CHECK_THROW(table.at(w, j), elle::Error);
  }
  BOOST_TEST(!table.contains({-1, 0}));
  BOOST_CHECK_THROW(table.at(-1, 0), elle::Error);
  BOOST_TEST(!table.contains({0, -1}));
  BOOST_CHECK_THROW(table.at(0, -1), elle::Error);
}

auto constexpr alignment = 64;

struct alignas(64) Aligned
{
  char c;
};

static
void
align()
{
  auto constexpr w = 2;
  auto constexpr h = 2;
  elle::Table<Aligned, 2> table(w, h);
  for (auto i : boost::irange(0, w))
    for (auto j : boost::irange(0, h))
      BOOST_TEST(std::size_t(&table.at(i, j)) % alignment == 0);
}

struct Beacon
{
  static int instances;

  Beacon(std::string s)
    : str(s)
  {
    ELLE_TRACE("{}: increase Beacon count", this);
    ++instances;
  };

  Beacon(Beacon const& b)
    : Beacon(b.str)
  {};

  ~Beacon()
  {
    ELLE_TRACE("{}: decrease Beacon count", this);
    --instances;
  };

  std::string str;
};

int Beacon::instances = 0;

static
void
initializer()
{
  {
    elle::Table<Beacon, 2> table{
      {Beacon("0"), Beacon("1"), Beacon("2")},
      {Beacon("3"), Beacon("4"), Beacon("5")},
    };
    BOOST_TEST(table.dimensions() == std::tuple(2, 3));
    BOOST_TEST(table.at(0, 0).str == "0");
    BOOST_TEST(table.at(0, 1).str == "1");
    BOOST_TEST(table.at(0, 2).str == "2");
    BOOST_TEST(table.at(1, 0).str == "3");
    BOOST_TEST(table.at(1, 1).str == "4");
    BOOST_TEST(table.at(1, 2).str == "5");
  }
  BOOST_TEST(Beacon::instances == 0);
  {
    BOOST_CHECK_THROW(
      (elle::Table<Beacon, 2>{
        {Beacon("0"), Beacon("1"), Beacon("2")},
        {Beacon("3"), Beacon("4"), Beacon("5"), Beacon("6")},
      }), elle::Error);
    BOOST_TEST(Beacon::instances == 0);
    BOOST_CHECK_THROW(
      (elle::Table<Beacon, 2>{
        {Beacon("0"), Beacon("1"), Beacon("2")},
        {Beacon("3"), Beacon("4")},
      }), elle::Error);
    BOOST_TEST(Beacon::instances == 0);
  }
}

struct Thrower
  : public Beacon
{
  Thrower(bool ctor, bool dtor)
    : Beacon("")
    , _ctor(ctor)
    , _dtor(dtor)
  {}

  Thrower(Thrower const& t)
    : Beacon("")
    , _ctor(t._ctor)
    , _dtor(t._dtor)
  {
    if (this->_ctor)
    {
      ELLE_TRACE("{}: abort Thrower constructor", this);
      elle::err("nope");
    }
    elle::unconst(t)._dtor = false;
  }

  ~Thrower() noexcept(false)
  {
    if (this->_dtor)
    {
      ELLE_TRACE("{}: abort Thrower destructor", this);
      elle::err("nope");
    }
  }

  ELLE_ATTRIBUTE(bool, ctor);
  ELLE_ATTRIBUTE(bool, dtor);
};

static
void
exceptions()
{
  {
    BOOST_CHECK_THROW((elle::Table<Thrower, 2>({
        {{false, false}, {false, false}, {false, false}},
        {{false, false}, {true, false}, {true, false}},
      })), elle::Error);
    BOOST_TEST(Beacon::instances == 0);
  }
  // FIXME: this cannot be implemented with Tables being backed by a
  // `std::vector`
  // BOOST_CHECK_THROW(([&]
  //                    {
  //                      elle::Table<Thrower, 2> t = {
  //                        {{false, false}, {false, false}, {false, false}},
  //                        {{false, false}, {false, true}, {false, true}},
  //                      };
  //                      BOOST_TEST(Beacon::instances == 6);
  //                    }()), elle::Error);
  // BOOST_TEST(Beacon::instances == 0);
}

static
void
copy()
{
  auto p = std::make_shared<int>(0);
  {
    elle::Table<std::shared_ptr<int>, 2> t1({{p, p}, {p, p}});
    auto t2 = t1;
    BOOST_TEST(p.use_count() == 9);
  }
  BOOST_TEST(p.use_count() == 1);
}

static
void
iteration()
{
  auto constexpr w = 2;
  auto constexpr h = 3;
  auto constexpr d = 4;
  elle::Table<int, 3> t(w, h, d);
  {
    int i = 0;
    for (auto e: t)
      e.second = i++;
    BOOST_TEST(i == 24);
  }
  auto it = t.begin();
  for (auto i : boost::irange(0, w))
    for (auto j : boost::irange(0, h))
      for (auto k : boost::irange(0, d))
      {
        auto v = h * d * i + d * j + k;
        BOOST_TEST(t.at(i, j, k) == v);
        BOOST_TEST((*it).first == std::tuple(i, j, k));
        BOOST_TEST((*it).second == v);
        ++it;
      }
  {
    int i = 0;
    auto const& ct = t;
    for (auto e: ct)
      BOOST_TEST(e.second == i++);
  }
  {
    int i = 0;
    for (auto& e: t.elements())
      e = i++;
    BOOST_TEST(i == 24);
  }
  {
    auto const& ct = t;
    int i = 0;
    for (auto const& e: ct.elements())
      BOOST_TEST(e == i++);
  }
  {
    // Check we can iterate on booleans, as the underlying vector packs them.
    elle::Table<bool, 2> t = {{true, true}};
    for (auto b: t.elements())
      BOOST_TEST(b);
    for (auto e: t)
      BOOST_TEST(std::get<1>(e));
  }
}

static
void
table_index()
{
  elle::Table<int, 3> t(2, 3, 4);
  BOOST_TEST(t.index({0, 0, 0}) == 0);
  BOOST_TEST(t.index({1, 0, 0}) == 12);
  BOOST_TEST(t.index({2, 0, 0}) == 24);
  BOOST_TEST(t.index({0, 1, 0}) == 4);
  BOOST_TEST(t.index({0, 1, 1}) == 5);
  BOOST_TEST(t.index({0, 0, 3}) == 3);
}

static
void
assign()
{
  auto p = std::make_shared<int>();
  elle::Table<std::shared_ptr<int>, 2> t = {
    {p, p},
    {p, p},
  };
  t = elle::Table<std::shared_ptr<int>, 2>(3, 3);
  BOOST_TEST(t.dimensions() == std::tuple(3, 3));
  BOOST_TEST(p.use_count() == 1);
  BOOST_TEST(!t.at(0, 0));
  BOOST_TEST(!t.at(1, 1));
  BOOST_TEST(!t.at(2, 2));
}

static
void
compare()
{
  elle::Table<int, 2> t1 = {
    {0, 1},
    {2, 3},
  };
  BOOST_TEST(t1 == t1);
  auto t2 = t1;
  BOOST_TEST(t1 == t2);
  BOOST_TEST(t2 == t1);
  t2.at(1, 0) = 4;
  BOOST_TEST(t1 != t2);
  BOOST_TEST(t2 != t1);
  t1.at(1, 0) = 4;
  BOOST_TEST(t1 == t2);
  BOOST_TEST(t2 == t1);
}


static
void
print()
{
  elle::Table<int, 2> t = {
    {0, 1, 2},
    {3, 4, 5},
  };
  BOOST_TEST(elle::print("{}", t) == "[[0, 1, 2], [3, 4, 5]]");
}

// Required so we can bounce to ADL definitions of get(<tuple_like>).
template <std::size_t I, typename T>
auto
get(T&& v)
  -> decltype(std::get<I>(std::forward<T>(v)))
{
  return std::get<I>(std::forward<T>(v));
}

static
void
resize()
{
  auto p1 = std::make_shared<int>();
  auto p2 = std::make_shared<int>();
  elle::Table<std::shared_ptr<int>, 2> t = {
    {p1, p2, p1},
    {p2, p1, p2},
    {p1, p2, p1},
  };
  auto const odd =
    [] (std::array<int, 2> i)
    {
      return bool((get<0>(i) + get<1>(i)) % 2);
    };
  for (auto const& e: t)
    BOOST_TEST(e.second == (odd(e.first) ? p2 : p1));
  t.dimensions({4, 4});
  BOOST_TEST(t.dimensions() == std::tuple(4, 4));
  BOOST_TEST(t.size() == 16);
  BOOST_TEST(p1.use_count() == 6);
  BOOST_TEST(p2.use_count() == 5);
  for (auto const& e: t)
    if (std::get<0>(e.first) > 2 || std::get<1>(e.first) > 2)
      BOOST_TEST(e.second == nullptr);
    else
      BOOST_TEST(e.second == (odd(e.first) ? p2 : p1));
  t.dimensions({2, 2});
  BOOST_TEST(t.dimensions() == std::tuple(2, 2));
  BOOST_TEST(t.size() == 4);
  BOOST_TEST(p1.use_count() == 3);
  BOOST_TEST(p2.use_count() == 3);
  for (auto const& e: t)
    BOOST_TEST(e.second == (odd(e.first) ? p2 : p1));
}

namespace serialization
{
  template <typename Format>
  static
  void
  serialization()
  {
    using T = elle::Table<int, 2>;
    T t = {
      {0, 1},
      {2, 3},
      {4, 5},
    };
    auto s = elle::serialization::serialize<Format>(t);
    ELLE_LOG("serialized: {}", s);
    auto u = elle::serialization::deserialize<Format, T>(s);
    BOOST_TEST(u == t);
  }

  static
  void
  errors()
  {
    using T = elle::Table<int, 2>;
    auto const fmt = R"(
      \{
        "dimensions": [2, 2],
        "elements": [{}]
      \})";
    BOOST_CHECK_THROW(
      elle::serialization::json::deserialize<T>(
        elle::Buffer(elle::print(fmt, "0, 1, 2")), false),
      elle::Error);
    elle::serialization::json::deserialize<T>(
      elle::Buffer(elle::print(fmt, "0, 1, 2, 3")), false);
    BOOST_CHECK_THROW(
      elle::serialization::json::deserialize<T>(
        elle::Buffer(elle::print(fmt, "0, 1, 2, 3, 4")), false),
      elle::Error);
  }
}

static
void
non_default_constructible()
{
  static_assert(!std::is_default_constructible<Beacon>::value);
  elle::Table<Beacon, 2> table(2, 2, Beacon("initial"));
  BOOST_TEST(Beacon::instances == 4);
  table.dimensions({3, 2}, Beacon("extended"));
  BOOST_TEST(Beacon::instances == 6);
  table.dimensions({3, 1}, Beacon("unused"));
  BOOST_TEST(Beacon::instances == 3);
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(orientation));
  master.add(BOOST_TEST_CASE(pod));
  master.add(BOOST_TEST_CASE(object));
  master.add(BOOST_TEST_CASE(bounds));
  master.add(BOOST_TEST_CASE(align));
  master.add(BOOST_TEST_CASE(initializer));
  master.add(BOOST_TEST_CASE(exceptions));
  master.add(BOOST_TEST_CASE(copy));
  master.add(BOOST_TEST_CASE(iteration));
  master.add(BOOST_TEST_CASE(table_index));
  master.add(BOOST_TEST_CASE(assign));
  master.add(BOOST_TEST_CASE(compare));
  master.add(BOOST_TEST_CASE(print));
  master.add(BOOST_TEST_CASE(resize));
 {
    boost::unit_test::test_suite* s = BOOST_TEST_SUITE("serialization");
    master.add(s);
    auto json = &serialization::serialization<elle::serialization::Json>;
    s->add(BOOST_TEST_CASE(json));
    auto binary = &serialization::serialization<elle::serialization::Binary>;
    s->add(BOOST_TEST_CASE(binary));
    using namespace serialization;
    s->add(BOOST_TEST_CASE(errors));
  }
  master.add(BOOST_TEST_CASE(non_default_constructible));
}
