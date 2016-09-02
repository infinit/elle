#include <elle/Option.hh>
#include <elle/log.hh>
#include <elle/serialization/binary.hh>
#include <elle/serialization/json.hh>
#include <elle/test.hh>

class Count
{
public:
  Count(int& count)
    : count(count)
    , dec(true)
    , destroyed(false)
  {
    ++count;
  }

  Count(Count&& source)
    : count(source.count)
    , dec(true)
    , destroyed(false)
  {
    source.dec = false;
  }

  Count(Count const& source)
    : count(source.count)
    , dec(true)
    , destroyed(false)
  {
    ++count;
  }

  ~Count()
  {
    BOOST_CHECK(!this->destroyed);
    if (this->dec)
      --count;
    this->destroyed = true;
  }

  int& count;
  bool dec;
  bool destroyed;

};

static
std::ostream&
operator << (std::ostream& o, Count const& c)
{
  return o << "Count(" << c.count << ")";
}

static
void
option()
{
  {
    elle::Option<int, std::string> opt(42);
    BOOST_CHECK(opt.is<int>());
    BOOST_CHECK_EQUAL(opt.get<int>(), 42);
    BOOST_CHECK(!opt.is<std::string>());
    BOOST_CHECK_THROW(opt.get<std::string>(), elle::Error);
  }
  {
    std::string data = "no rage de mon bolossage";
    elle::Option<int, std::string> opt(data);
    BOOST_CHECK(opt.is<std::string>());
    BOOST_CHECK_EQUAL(opt.get<std::string>(), data);
    BOOST_CHECK(!opt.is<int>());
    BOOST_CHECK_THROW(opt.get<int>(), elle::Error);
  }
  {
    int count = 0;
    {
      elle::Option<Count> opt{Count(count)};
      BOOST_CHECK_EQUAL(count, 1);
    }
    BOOST_CHECK_EQUAL(count, 0);
  }
}

class Nope
{
public:
  Nope(int i_)
    : i(i_)
  {}

  Nope(Nope&& source)
    : i(source.i)
  {
    --source.i;
  }

  Nope(Nope const&) = delete;
  int i;
};

static
std::ostream&
operator << (std::ostream& o, Nope const& n)
{
  return o << "Nope(" << n.i << ")";
}


static
void
construction()
{
  // move
  {
    elle::Option<int, Nope> opt{Nope(42)};
    elle::Option<int, Nope> moved{std::move(opt)};
    BOOST_CHECK_EQUAL(opt.get<Nope>().i, 41);
    BOOST_CHECK_EQUAL(moved.get<Nope>().i, 42);
  }
  // copy
  {
    std::vector<int> v{0, 1, 2};
    elle::Option<int, std::vector<int>> opt{v};
    elle::Option<int, std::vector<int>> copy{opt};
    BOOST_CHECK_EQUAL(opt.get<std::vector<int>>(), v);
    BOOST_CHECK_EQUAL(copy.get<std::vector<int>>(), v);
  }
}

static
void
assignment()
{
  int count = 0;
  // move
  {
    elle::Option<int, Count> opt{Count(count)};
    elle::Option<int, Count> assigned{Count(count)};
    BOOST_CHECK_EQUAL(count, 2);
    assigned = std::move(opt);
    BOOST_CHECK_EQUAL(count, 1);
  }
  BOOST_CHECK_EQUAL(count, 0);
  // copy
  {
    BOOST_CHECK_EQUAL(count, 0);
    elle::Option<int, Count> opt{Count(count)};
    elle::Option<int, Count> assigned{Count(count)};
    BOOST_CHECK_EQUAL(count, 2);
    assigned = opt;
    BOOST_CHECK_EQUAL(count, 2);
  }
  BOOST_CHECK_EQUAL(count, 0);
}

static
void
reset()
{
  {
    int count = 0;
    BOOST_CHECK_EQUAL(count, 0);
    elle::Option<Count, Nope> opt{Count(count)};
    BOOST_CHECK_EQUAL(count, 1);
    opt.emplace(Count(count));
    BOOST_CHECK_EQUAL(count, 1);
    opt.emplace(Nope(42));
    BOOST_CHECK_EQUAL(count, 0);
    opt.emplace(Count(count));
    BOOST_CHECK_EQUAL(count, 1);
  }
  {
    int count = 0;
    elle::Option<Count, Nope> opt = elle::Option<Count, Nope>(Count(count));
    BOOST_CHECK_EQUAL(count, 1);
    opt = Nope(42);
    BOOST_CHECK_EQUAL(count, 0);
  }
}

static
void
print()
{
  elle::Option<int, std::string> i(42);
  BOOST_CHECK_EQUAL(elle::sprintf("%s", i), "42");
  elle::Option<int, std::string> s(std::string("quarante deux"));
  BOOST_CHECK_EQUAL(elle::sprintf("%s", s), "quarante deux");
}

template <int I>
class Checker
{
public:
  Checker()
  {
    ++this->count;
  }

  Checker(Checker const&) = delete;

  Checker(Checker&&)
  {
    ++this->count;
  }

  ~Checker()
  {
    --this->count;
  }

  void
  serialize(elle::serialization::Serializer&)
  {}

  static int count;
};

template <int I>
int Checker<I>::count = 0;

template <typename Format, typename Opt>
static
void
_serialization_inplace()
{
  {
    Checker<0>::count = 0;
    Checker<1>::count = 0;
    Opt o{Checker<1>()};
    BOOST_CHECK_EQUAL(Checker<1>::count, 1);
    auto data = elle::serialization::serialize<Format>(Opt{Checker<0>()}, false);
    BOOST_CHECK_EQUAL(Checker<0>::count, 0);
    elle::IOStream s(data.istreambuf());
    typename Format::SerializerIn input(s, false);
    o.serialize(input);
    BOOST_CHECK_EQUAL(Checker<0>::count, 1);
    BOOST_CHECK_EQUAL(Checker<1>::count, 0);
  }
  BOOST_CHECK_EQUAL(Checker<0>::count, 0);
  BOOST_CHECK_EQUAL(Checker<1>::count, 0);
}


template <typename Format>
static
void
_serialization()
{
  {
    using Opt = elle::Option<int, std::string>;
    {
      Opt i(42);
      BOOST_CHECK_EQUAL(
        (elle::serialization::deserialize<Format, Opt>(
          elle::serialization::serialize<Format>(i))).template get<int>(),
        42);
    }
    {
      Opt s(std::string("quarante deux"));
      BOOST_CHECK_EQUAL(
        (elle::serialization::deserialize<Format, Opt>(
          elle::serialization::serialize<Format>(s))).template get<std::string>(),
        "quarante deux");
    }
  }
  _serialization_inplace<Format, elle::Option<Checker<0>, Checker<1>>>();
  _serialization_inplace<Format, elle::Option<Checker<1>, Checker<0>>>();
}

static
void
serialization()
{
  _serialization<elle::serialization::Binary>();
  _serialization<elle::serialization::Json>();
}

static
void
serialization2()
{
  typedef elle::Option<int, std::shared_ptr<int>> O;
  O o1(std::make_shared<int>(42));
  auto s = elle::serialization::binary::serialize(o1, "option");
  {
    // Fry that stack.
    char buf[sizeof(O)];
    memset(buf, 0xd0, sizeof(O));
  }
  O o2(42);
  {
    elle::IOStream input(s.istreambuf());
    elle::serialization::binary::SerializerIn s(input);
    s.serialize("option", o2);
  }
  BOOST_CHECK(o2.is<std::shared_ptr<int>>());
  BOOST_CHECK_EQUAL(*o2.get<std::shared_ptr<int>>(), 42);
}

class Thrower
{
public:
  explicit
  Thrower()
    : t(false)
  {}

  Thrower(Thrower&& t)
  {
    if (t.t)
      throw elle::Error("Thrower");
  }

  bool t;
};

static
void
exceptions()
{
  typedef elle::Option<Count, Thrower> O;
  int count = 0;
  {
    O o{Count(count)};
    BOOST_CHECK_EQUAL(count, 1);
    O thrower{Thrower()};
    thrower.get<Thrower>().t = true;
    BOOST_CHECK_THROW(o = std::move(thrower), elle::Error);
    BOOST_CHECK_EQUAL(count, 0);
  }
  BOOST_CHECK_EQUAL(count, 0);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(option));
  suite.add(BOOST_TEST_CASE(construction));
  suite.add(BOOST_TEST_CASE(assignment));
  suite.add(BOOST_TEST_CASE(reset));
  suite.add(BOOST_TEST_CASE(print));
  suite.add(BOOST_TEST_CASE(serialization));
  suite.add(BOOST_TEST_CASE(serialization2));
  suite.add(BOOST_TEST_CASE(exceptions));
}
