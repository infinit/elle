#define BOOST_TEST_DYN_LINK

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

static
void
test_lambda()
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

static
void
test_delete()
{
  TestObject* obj = new TestObject;
  BOOST_CHECK(TestObject::i == 1);
  {
    ELLE_FINALLY_ACTION_DELETE(obj);
  }
  BOOST_CHECK(TestObject::i == 0);
}

static
void
test_delete_array()
{
  TestObject* arr = new TestObject[10];
  BOOST_CHECK(TestObject::i == 10);
  {
    ELLE_FINALLY_ACTION_DELETE_ARRAY(arr);
  }
  BOOST_CHECK(TestObject::i == 0);
}

// XXX No test for free as it's not possible

static
void
test_abort()
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

static
bool
test_suite()
{
  boost::unit_test::test_suite* basics = BOOST_TEST_SUITE("Basics");
  boost::unit_test::framework::master_test_suite().add(basics);
  basics->add(BOOST_TEST_CASE(std::bind(test_lambda)));
  basics->add(BOOST_TEST_CASE(std::bind(test_delete)));
  basics->add(BOOST_TEST_CASE(std::bind(test_delete_array)));
  basics->add(BOOST_TEST_CASE(std::bind(test_abort)));

	return true;
}

int
main(int argc, char** argv)
{
  return ::boost::unit_test::unit_test_main(test_suite, argc, argv);
}