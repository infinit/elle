#define BOOST_TEST_MODULE With

#include <elle/With.hh>
#include <elle/test.hh>

class GoodBoy
{
public:
  GoodBoy(int i)
    : _i(i)
  {}

  ~GoodBoy()
  {
    BOOST_CHECK(!this->_destructed);
    this->_destructed = true;
  }

private:
  ELLE_ATTRIBUTE_RW(int, i);
  bool _destructed = false;
};

class BadBoy
{
public:
  BadBoy(int i)
    : _i(i)
  {}

  BadBoy(BadBoy&& source)
    : _i(source._i)
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
  bool _destructed = false;
};

class CheckDestruct
{
public:
  CheckDestruct(bool& beacon)
    : _beacon(beacon)
  {}

  ~CheckDestruct()
  {
    this->_beacon = true;
  }

private:
  ELLE_ATTRIBUTE(bool&, beacon);
};

BOOST_AUTO_TEST_CASE(normal)
{
  bool beacon = false;
  elle::With<GoodBoy>(3) << [&] (GoodBoy& g)
  {
    BOOST_CHECK_EQUAL(g.i(), 3);
    beacon = true;
  };
  BOOST_CHECK(beacon);
}

BOOST_AUTO_TEST_CASE(exception)
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

BOOST_AUTO_TEST_CASE(double_exception)
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

BOOST_AUTO_TEST_CASE(value)
{
  BOOST_CHECK_EQUAL(
    elle::With<GoodBoy>(1660) << [&] (GoodBoy& g)
    {
      return g.i() + 4;
    },
    1664);
}

namespace
{
  elle::With<BadBoy>
  make_with(int i)
  {
    return i;
  }
}

BOOST_AUTO_TEST_CASE(move)
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

BOOST_AUTO_TEST_CASE(unused)
{
  bool beacon = false;
  elle::With<CheckDestruct>{beacon};
  BOOST_CHECK(beacon);
}

// The example from the Doxygen documentation.
namespace
{
  struct DThrower
  {
    DThrower(bool destructor_throws)
      : _destructor_throws(destructor_throws)
    {}
    ~DThrower() noexcept(false)
    {
      if (_destructor_throws)
        throw 1;
    };
    bool _destructor_throws;
  };

  int
  test(bool body_throws, bool destructor_throws)
  {
    try
    {
      elle::With<DThrower>(destructor_throws) << [&] (DThrower& d)
        {
          throw body_throws ? 2 : 3;
        };
    }
    catch (int value)
    {
      return value;
    }
    elle::unreachable();
  }

  BOOST_AUTO_TEST_CASE(doc)
  {
    BOOST_TEST(test(false, false) == 3);
    BOOST_TEST(test(true, false) == 2);
    BOOST_TEST(test(false, true) == 1);
    BOOST_TEST(test(true, true) == 1);
  }
}
