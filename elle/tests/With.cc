#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE With
#include <boost/test/unit_test.hpp>

#include <elle/With.hh>

class GoodBoy
{
public:
  GoodBoy():
    _destructed(false)
  {}

  ~GoodBoy()
  {
    BOOST_CHECK(!this->_destructed);
    this->_destructed = true;
  }
private:
  bool _destructed;
};

class BadBoy
{
  public:
    BadBoy(int i):
      _i(i),
      _destructed(false)
    {}

    ~BadBoy() noexcept(false)
    {
      BOOST_CHECK(!this->_destructed);
      this->_destructed = true;
      throw this->_i;
    }

  private:
    int _i;
    bool _destructed;
};

BOOST_AUTO_TEST_CASE(normal)
{
  bool beacon = false;
  elle::With<GoodBoy>() << [&] (GoodBoy& b)
  {
    beacon = true;
  };
  BOOST_CHECK(beacon);
}

BOOST_AUTO_TEST_CASE(exception)
{
  bool beacon = false;
  try
  {
    elle::With<BadBoy>(42) << [&] (BadBoy& b)
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
    elle::With<BadBoy>(51) << [&] (BadBoy& b)
    {
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
    elle::With<GoodBoy>() << [&] (GoodBoy&)
    {
      return 1664;
    },
    1664);
}
