#define BOOST_TEST_MODULE cast
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <elle/cast.hh>

struct A
{
  virtual ~A() {};
};
struct B: A {};
struct C: B {};

BOOST_AUTO_TEST_CASE(cast_dynamic1)
{
  auto b = std::unique_ptr<B>(new B);
  std::unique_ptr<A> a = elle::cast<A>::runtime(b);
  BOOST_CHECK(bool(b) == false);
  BOOST_CHECK(bool(a) == true);
}

BOOST_AUTO_TEST_CASE(cast_dynamic2)
{
  std::unique_ptr<B> b = std::unique_ptr<B>(new B);
  std::unique_ptr<A> a = elle::cast<A>::runtime(b); // works because B is a A
  std::unique_ptr<C> c = elle::cast<C>::runtime(a); // don't work because B is not a C
  BOOST_CHECK(bool(a) == true);
  BOOST_CHECK(bool(b) == false);
  BOOST_CHECK(bool(c) == false);
}
