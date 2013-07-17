#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE finally

#include <elle/finally.hh>

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
  {
    ELLE_FINALLY_LAMBDA(test, [] (int* s)
                              {
                                ++*s;
                              });
    BOOST_CHECK(*test == 5);
  }
  BOOST_CHECK(*test == 6);
  delete test;
}

BOOST_AUTO_TEST_CASE(test_delete)
{
  TestObject* obj = new TestObject;
  BOOST_CHECK(TestObject::i == 1);
  {
    ELLE_FINALLY_ACTION_DELETE(obj);
  }
  BOOST_CHECK(TestObject::i == 0);
}

BOOST_AUTO_TEST_CASE(test_delete_array)
{
  TestObject* arr = new TestObject[10];
  BOOST_CHECK(TestObject::i == 10);
  {
    ELLE_FINALLY_ACTION_DELETE_ARRAY(arr);
  }
  BOOST_CHECK(TestObject::i == 0);
}

// XXX No test for free as it's not possible

BOOST_AUTO_TEST_CASE(test_abort)
{
  TestObject* obj = new TestObject;
  BOOST_CHECK(TestObject::i == 1);
  {
    ELLE_FINALLY_ACTION_DELETE(obj);
    ELLE_FINALLY_ABORT(obj);
  }
  BOOST_CHECK(TestObject::i == 1);
  delete obj;
}

BOOST_AUTO_TEST_CASE(test_advanced)
{
  int i = 1;
  {
    ELLE_AT_SCOPE_EXIT {
      i--;
    };
  }
  BOOST_CHECK_EQUAL(i, 0);
}

