#include <elle/Option.hh>
#include <elle/test.hh>
#include <elle/log.hh>

class Count
{
public:
  Count(int& count)
    : count(count)
    , dec(true)
  {
    ++count;
  }

  Count(Count&& source)
    : count(source.count)
    , dec(true)
  {
    source.dec = false;
  }

  Count(Count const& source)
    : count(source.count)
    , dec(true)
  {
    ++count;
  }

  ~Count()
  {
    if (this->dec)
      --count;
  }

  int& count;
  bool dec;
};

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

static
void
print()
{
  elle::Option<int, std::string> i(42);
  BOOST_CHECK_EQUAL(elle::sprintf("%s", i), "42");
  elle::Option<int, std::string> s(std::string("quarante deux"));
  BOOST_CHECK_EQUAL(elle::sprintf("%s", s), "quarante deux");
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(option));
  suite.add(BOOST_TEST_CASE(construction));
  suite.add(BOOST_TEST_CASE(assignment));
  suite.add(BOOST_TEST_CASE(reset));
  suite.add(BOOST_TEST_CASE(print));
}
