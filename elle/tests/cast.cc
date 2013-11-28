#include <elle/cast.hh>
#include <elle/test.hh>

struct A
{
  virtual ~A() {};
};
struct B: A {};
struct C: B {};

static
void
cast_dynamic1()
{
  auto b = std::unique_ptr<B>(new B);
  std::unique_ptr<A> a = elle::cast<A>::runtime(b);
  BOOST_CHECK(bool(b) == false);
  BOOST_CHECK(bool(a) == true);
}

static
void
cast_dynamic2()
{
  std::unique_ptr<B> b = std::unique_ptr<B>(new B);
  std::unique_ptr<A> a = elle::cast<A>::runtime(b); // works because B is a A
  std::unique_ptr<C> c = elle::cast<C>::runtime(a); // don't work because B is not a C
  BOOST_CHECK(bool(a) == true);
  BOOST_CHECK(bool(b) == false);
  BOOST_CHECK(bool(c) == false);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();

  suite.add(BOOST_TEST_CASE(cast_dynamic1));
  suite.add(BOOST_TEST_CASE(cast_dynamic2));
}
