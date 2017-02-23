#include <elle/With.hh>
#include <elle/finally.hh>
#include <elle/test.hh>

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
  elle::With<elle::Finally>([&] { ++*test; }) << [&]
  {
    BOOST_CHECK_EQUAL(*test, 5);
  };
  BOOST_CHECK_EQUAL(*test, 6);
  delete test;
}

static
void
test_delete()
{
  TestObject* obj = new TestObject;
  elle::With<elle::Finally>([&] { delete obj; }) << [&]
  {
    BOOST_CHECK(TestObject::i == 1);
  };
  BOOST_CHECK(TestObject::i == 0);
}

static
void
test_abort()
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

static
void
test_aborted()
{
  bool beacon = true;
  {
    elle::SafeFinally finally([&] { beacon = false; });
    BOOST_CHECK_EQUAL(finally.aborted(), false);
    finally.abort();
    BOOST_CHECK_EQUAL(finally.aborted(), true);
  }
  BOOST_CHECK(beacon);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(test_lambda));
  suite.add(BOOST_TEST_CASE(test_delete));
  suite.add(BOOST_TEST_CASE(test_abort));
  suite.add(BOOST_TEST_CASE(test_aborted));
}
