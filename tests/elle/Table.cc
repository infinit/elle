#include <elle/Table.hh>

#include <string>

#include <boost/range/irange.hpp>

#include <elle/log.hh>
#include <elle/range.hh>
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
    BOOST_CHECK_THROW(table.at(i, h), elle::Error);
  for (auto j : boost::irange(0, h))
    BOOST_CHECK_THROW(table.at(w, j), elle::Error);
  BOOST_CHECK_THROW(table.at(-1, 0), elle::Error);
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
    BOOST_TEST(table.dimensions() == std::make_tuple(2, 3));
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
  BOOST_CHECK_THROW(([&]
                     {
                       elle::Table<Thrower, 2> t = {
                         {{false, false}, {false, false}, {false, false}},
                         {{false, false}, {false, true}, {false, true}},
                       };
                       BOOST_TEST(Beacon::instances == 6);
                     }()), elle::Error);
  BOOST_TEST(Beacon::instances == 0);
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
  int i = 0;
  for (auto e: t)
    e.second = i++;
  BOOST_TEST(i == 24);
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
  i = 0;
  auto const& ct = t;
  for (auto e: ct)
    BOOST_TEST(e.second == i++);
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
}
