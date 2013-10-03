#define BOOST_TEST_MODULE finally

#include <elle/finally.hh>
#include <elle/With.hh>

#include <boost/test/unit_test.hpp>

class TestObject
{
public:
  static int i;

  TestObject()
  {
    ++i;
  }

  ~TestObject()
  {
    --i;
  }

};

int TestObject::i = 0;

BOOST_AUTO_TEST_CASE(test_lambda)
{
  int* test = new int(5);
  elle::With<elle::Finally>([&] { ++*test; }) << [&]
  {
    BOOST_CHECK_EQUAL(*test, 5);
  };
  BOOST_CHECK_EQUAL(*test, 6);
  delete test;
}

BOOST_AUTO_TEST_CASE(test_delete)
{
  TestObject* obj = new TestObject;
  elle::With<elle::Finally>([&] { delete obj; }) << [&]
  {
    BOOST_CHECK(TestObject::i == 1);
  };
  BOOST_CHECK(TestObject::i == 0);
}

BOOST_AUTO_TEST_CASE(test_abort)
{
  TestObject* obj = new TestObject;
  elle::With<elle::Finally>([&] { delete obj; }) << [&] (elle::Finally& f)
  {
    BOOST_CHECK(TestObject::i == 1);
    f.abort();
  };
  BOOST_CHECK(TestObject::i == 1);
  delete obj;
  BOOST_CHECK(TestObject::i == 0);
}
