#include <elle/memory.hh>
#include <elle/test.hh>
#include <tuple>

static
void
make_unique()
{
  std::unique_ptr<std::tuple<int, char, double>> ptr =
    elle::make_unique<std::tuple<int, char, double>>(1, 'c', 1.0f);
  BOOST_CHECK(std::get<0>(*ptr) == 1);
  BOOST_CHECK(std::get<1>(*ptr) == 'c');
  BOOST_CHECK(std::get<2>(*ptr) == 1.0f);

  std::unique_ptr<std::tuple<>> empty_ptr =
    elle::make_unique<std::tuple<>>();
}

static
void
generic_unique_ptr()
{
  elle::generic_unique_ptr<int> empty;
  elle::generic_unique_ptr<int> null(nullptr);
  elle::generic_unique_ptr<int> simple(new int);
  elle::generic_unique_ptr<int> array(new int[10],
                                      std::function<void (int*)>(
                                        [] (int* p)
                                        { delete [] p; }));
  elle::generic_unique_ptr<int> array_erasure(new int[10],
                                              std::default_delete<int[]>());
  elle::generic_unique_ptr<int> std_simple(std::unique_ptr<int>(new int));
  elle::generic_unique_ptr<int> std_array(std::unique_ptr<int[]>(new int[10]));
  elle::generic_unique_ptr<int> copy(
    elle::generic_unique_ptr<int>(new int));

  {
    auto raw = new int[10];
    elle::generic_unique_ptr<int> assign(new int);
    assign = std::unique_ptr<int[]>(raw);
    BOOST_CHECK_EQUAL(assign.get(), raw);
  }
}

struct Super
{
  Super(int& count)
    : _count(count)
  {
    ++this->_count;
  }

  virtual
  ~Super()
  {
    --this->_count;
  }

  int& _count;
};

struct Child
  : public Super
{
  Child(int& count)
    : Super(count)
  {
    ++this->_count;
  }

  virtual
  ~Child()
  {
    --this->_count;
  }
};

static
void
dynamic_pointer_cast()
{
  {
    int count;
    BOOST_CHECK(
      !std::dynamic_pointer_cast<Child>(
        std::unique_ptr<Super>(new Super(count))));
    BOOST_CHECK(
      std::dynamic_pointer_cast<Child>(
        std::unique_ptr<Super>(new Child(count))));
  }
  {
    int count = 0;
    {
      auto super = elle::make_unique<Super>(count);
      auto child = elle::make_unique<Child>(count);
      BOOST_CHECK_EQUAL(count, 3);
      BOOST_CHECK(super);
      BOOST_CHECK(!std::dynamic_pointer_cast<Child>(super));
      BOOST_CHECK(super);
      auto casted = std::dynamic_pointer_cast<Child>(child);
      BOOST_CHECK(casted);
      BOOST_CHECK(!child);
      BOOST_CHECK_EQUAL(count, 3);
    }
    BOOST_CHECK_EQUAL(count, 0);
  }
  {
    bool beacon = false;
    {
      int count = 0;
      std::unique_ptr<Child, std::function<void(Super*)>> child;
      auto deleter = [&] (Super* p) { delete p; beacon = true; };
      {
        auto super = std::unique_ptr<Super, std::function<void(Super*)>>
          (new Child(count), deleter);
        child = std::dynamic_pointer_cast<Child>(super);
        BOOST_CHECK(child);
        BOOST_CHECK(!super);
      }
      BOOST_CHECK(!beacon);
    }
    BOOST_CHECK(beacon);
  }
}

static
void
ambivalent_ptr()
{
  // shared_ptr to ambivalent_ptr
  {
    int count = 0;
    auto i = std::make_shared<Super>(count);
    BOOST_CHECK_EQUAL(count, 1);
    std::ambivalent_ptr<Super> a(i);
    BOOST_CHECK_EQUAL(count, 1);
    BOOST_CHECK(a.lock());
    i.reset();
    BOOST_CHECK_EQUAL(count, 0);
    BOOST_CHECK(!a.lock());
  }
  // ambivalent_ptr to shared_ptr
  {
    int count = 0;
    std::ambivalent_ptr<Super> a(new Super(count));
    BOOST_CHECK_EQUAL(count, 1);
    std::shared_ptr<Super> p(a.lock());
    BOOST_CHECK(a.lock());
    p.reset();
    BOOST_CHECK_EQUAL(count, 1);
    BOOST_CHECK(a.lock());
  }
  // copy ambivalent_ptr
  {
    int count = 0;
    std::ambivalent_ptr<Super> a(new Super(count));
    BOOST_CHECK_EQUAL(count, 1);
    std::ambivalent_ptr<Super> b(a);
    BOOST_CHECK(a.lock());
    BOOST_CHECK(b.lock());
    BOOST_CHECK_EQUAL(count, 1);
    a.payload().reset();
    BOOST_CHECK(a.lock());
    BOOST_CHECK(b.lock());
    BOOST_CHECK_EQUAL(count, 1);
    b.payload().reset();
    BOOST_CHECK(!a.lock());
    BOOST_CHECK(!b.lock());
    BOOST_CHECK_EQUAL(count, 0);
  }
  // move ambivalent_ptr
  {
    int count = 0;
    std::ambivalent_ptr<Super> a(new Super(count));
    BOOST_CHECK_EQUAL(count, 1);
    std::ambivalent_ptr<Super> b(std::move(a));
    BOOST_CHECK(!a.payload());
    BOOST_CHECK(b.lock());
    BOOST_CHECK_EQUAL(count, 1);
    b.payload().reset();
    BOOST_CHECK(!a.lock());
    BOOST_CHECK(!b.lock());
    BOOST_CHECK_EQUAL(count, 0);
  }
  // failed dynamic_cast
  {
    int count = 0;
    auto b = std::dynamic_pointer_cast<Child>(
      std::ambivalent_ptr<Super>(new Super(count)));
    BOOST_CHECK_EQUAL(count, 0);
    BOOST_CHECK(!b.lock());
  }
  // successful dynamic_cast
  {
    int count = 0;
    auto b = std::dynamic_pointer_cast<Child>(
      std::ambivalent_ptr<Super>(new Child(count)));
    BOOST_CHECK_EQUAL(count, 2);
    BOOST_CHECK(b.lock());
  }
  // reset
  {
    int count = 0;
    auto a = std::ambivalent_ptr<Super>::own(std::make_shared<Super>(count));
    BOOST_CHECK_EQUAL(count, 1);
    BOOST_CHECK(a.lock());
    a.reset();
    BOOST_CHECK_EQUAL(count, 0);
    BOOST_CHECK(!a.lock());
  }
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(make_unique));
  suite.add(BOOST_TEST_CASE(generic_unique_ptr));
  suite.add(BOOST_TEST_CASE(dynamic_pointer_cast));
  suite.add(BOOST_TEST_CASE(ambivalent_ptr));
}
