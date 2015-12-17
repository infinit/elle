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

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();

  suite.add(BOOST_TEST_CASE(make_unique));
  suite.add(BOOST_TEST_CASE(generic_unique_ptr));
  suite.add(BOOST_TEST_CASE(dynamic_pointer_cast));
}
