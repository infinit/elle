#include <elle/With.hh>
#include <elle/test.hh>

class GoodBoy
{
public:
  GoodBoy(int i):
    _i(i),
    _destructed(false)
  {}

  ~GoodBoy()
  {
    BOOST_CHECK(!this->_destructed);
    this->_destructed = true;
  }


private:
  ELLE_ATTRIBUTE_RW(int, i);
  bool _destructed;
};

class BadBoy
{
public:
  BadBoy(int i):
    _i(i),
    _destructed(false)
  {}

  BadBoy(BadBoy&& source):
    _i(source._i),
    _destructed(false)
  {
    source._i = 0;
  }

  ~BadBoy() noexcept(false)
  {
    BOOST_CHECK(!this->_destructed);
    this->_destructed = true;
    if (this->_i)
      throw this->_i;
  }

private:
  ELLE_ATTRIBUTE_RW(int, i);
  bool _destructed;
};

class CheckDestruct
{
public:
  CheckDestruct(bool& beacon):
    _beacon(beacon)
  {}

  ~CheckDestruct()
  {
    this->_beacon = true;
  }

private:
  ELLE_ATTRIBUTE(bool&, beacon);
};

static
void
normal()
{
  bool beacon = false;
  elle::With<GoodBoy>(3) << [&] (GoodBoy& g)
  {
    BOOST_CHECK_EQUAL(g.i(), 3);
    beacon = true;
  };
  BOOST_CHECK(beacon);
}

static
void
exception()
{
  bool beacon = false;
  try
  {
    elle::With<BadBoy>(42) << [&] ()
    {
      beacon = true;
    };
  }
  catch (int i)
  {
    BOOST_CHECK(beacon);
    BOOST_CHECK_EQUAL(i, 42);
    return;
  }
  BOOST_FAIL("should have thrown");
}

static
void
double_exception()
{
  bool beacon = false;
  try
  {
    elle::With<BadBoy>(42) << [&] (BadBoy& b)
    {
      b.i(51);
      beacon = true;
      throw 69;
    };
  }
  catch (int i)
  {
    BOOST_CHECK(beacon);
    BOOST_CHECK_EQUAL(i, 51);
    return;
  }
  BOOST_FAIL("should have thrown");
}

static
void
value()
{
  BOOST_CHECK_EQUAL(
    elle::With<GoodBoy>(1660) << [&] (GoodBoy& g)
    {
      return g.i() + 4;
    },
    1664);
}

static
elle::With<BadBoy>
make_with(int i)
{
  return elle::With<BadBoy>(i);
}

static
void
move()
{
  bool beacon = false;
  try
  {
    auto with = make_with(86);
    with << [&]
    {
      beacon = true;
    };
  }
  catch (int i)
  {
    BOOST_CHECK(beacon);
    BOOST_CHECK_EQUAL(i, 86);
    return;
  }
  BOOST_FAIL("should have thrown");
}

static
void
unused()
{
  bool beacon = false;
  elle::With<CheckDestruct>{beacon};
  BOOST_CHECK(beacon);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();

  suite.add(BOOST_TEST_CASE(normal));
  suite.add(BOOST_TEST_CASE(exception));
  suite.add(BOOST_TEST_CASE(double_exception));
  suite.add(BOOST_TEST_CASE(value));
  suite.add(BOOST_TEST_CASE(move));
  suite.add(BOOST_TEST_CASE(unused));
}
