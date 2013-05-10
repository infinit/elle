#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <elle/Exception.hh>

using elle::Exception;

class Attr
{
public:
  Attr(int i):
    i(i),
    copy(0)
  {}

  Attr(Attr const& other):
    i(other.i),
    copy(0)
  {
    ++other.copy;
  }

  Attr&
  operator = (Attr const& other)
  {
    this-> i = other.i;
    ++this->copy;
    ++other.copy;
    return *this;
  }

  int i;
  mutable int copy;
};

class Test
{
public:
  Test(int r, int w, int x):
    _r(r),
    _w(w),
    _x(x)
  {}

  ELLE_ATTRIBUTE_R(Attr, r);
  ELLE_ATTRIBUTE_RW(Attr, w);
  ELLE_ATTRIBUTE_RWX(Attr, x);
};

static
void
test_r()
{
  Test t(1, 2, 3);

  BOOST_CHECK_EQUAL(t.r().i, 1);
  BOOST_CHECK_EQUAL(t.r().copy, 0);
}


static
void
test_w()
{
  Test t(1, 2, 3);

  BOOST_CHECK_EQUAL(t.w().i, 2);
  BOOST_CHECK_EQUAL(t.w().copy, 0);
  t.w(42);
  BOOST_CHECK_EQUAL(t.w().i, 42);
  BOOST_CHECK_EQUAL(t.w().copy, 1);
  t.w(Attr(51));
  BOOST_CHECK_EQUAL(t.w().i, 51);
  BOOST_CHECK_EQUAL(t.w().copy, 2);
}

static
void
test_x()
{
  Test t(1, 2, 3);

  BOOST_CHECK_EQUAL(t.x().i, 3);
  BOOST_CHECK_EQUAL(t.x().copy, 0);
  t.x(42);
  BOOST_CHECK_EQUAL(t.x().i, 42);
  BOOST_CHECK_EQUAL(t.x().copy, 1);
  t.x() = Attr(51);
  BOOST_CHECK_EQUAL(t.x().i, 51);
  BOOST_CHECK_EQUAL(t.x().copy, 2);
}

static
bool
test_suite()
{
  boost::unit_test::test_suite* exn = BOOST_TEST_SUITE("attributes");
  boost::unit_test::framework::master_test_suite().add(exn);
  exn->add(BOOST_TEST_CASE(test_r));
  exn->add(BOOST_TEST_CASE(test_w));
  exn->add(BOOST_TEST_CASE(test_x));

  return true;
}

int
main(int argc, char** argv)
{
  return ::boost::unit_test::unit_test_main(test_suite, argc, argv);
}
