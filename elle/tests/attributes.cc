#include <thread>
#include <vector>

#include <elle/Exception.hh>
#include <elle/attribute.hh>
#include <elle/test.hh>
#include <elle/threading/rw-mutex.hh>

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

enum class dummy_enum
{};

class Object
{};

class Types
{
public:
  static int i;
  Types()
    : _integer_ref(i)
  {}

  ELLE_ATTRIBUTE_RWX(int, integer);
  ELLE_ATTRIBUTE_RWX(dummy_enum, enumeration);
  ELLE_ATTRIBUTE_RWX(int*, pointer);
  ELLE_ATTRIBUTE_RWX(int&, integer_ref);
  ELLE_ATTRIBUTE_RWX(Object, object);
};
int Types::i;


static
void
types()
{
  Types t;
  Types const& ct = t;
  BOOST_CHECK((std::is_same<decltype(ct.integer()), int>::value));
  BOOST_CHECK((std::is_same<decltype(t.integer()), int&>::value));
  BOOST_CHECK((std::is_same<decltype(ct.enumeration()), dummy_enum>::value));
  BOOST_CHECK((std::is_same<decltype(t.enumeration()), dummy_enum&>::value));
  BOOST_CHECK((std::is_same<decltype(ct.pointer()), int*>::value));
  BOOST_CHECK((std::is_same<decltype(t.pointer()), int*&>::value));
  BOOST_CHECK((std::is_same<decltype(ct.object()), Object const&>::value));
  BOOST_CHECK((std::is_same<decltype(t.object()), Object&>::value));
};

class ThreadSafe
{
public:
  ThreadSafe()
    : _ints()
  {}

  ELLE_ATTRIBUTE_RW_TS(std::vector<int>, ints);
};

static
void
thread_safe()
{
  ThreadSafe ts;
  std::thread writer(
    [&]
    {
      for (int i = 0; i < 1024; ++i)
        ts.ints(std::vector<int>({i}));
    });
  std::thread reader(
    [&]
    {
      while (ts.ints().size() == 0 || ts.ints()[0] != 1023)
        ;
    });
  writer.join();
  reader.join();
}

class Virtual
{
  ELLE_ATTRIBUTE_RWX(int, x, virtual);
};

class Child:
  public Virtual
{
  int
  x() const override;
  void
  x(int v) override;
  int&
  x() override;
};

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(test_r), 0, valgrind(3));
  suite.add(BOOST_TEST_CASE(test_w), 0, valgrind(3));
  suite.add(BOOST_TEST_CASE(test_x), 0, valgrind(3));
  suite.add(BOOST_TEST_CASE(types), 0, valgrind(3));
  suite.add(BOOST_TEST_CASE(thread_safe), 0, valgrind(10));
}
