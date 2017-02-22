# include <type_traits>

# include <elle/test.hh>

// enable_if_exists
template <typename T>
static constexpr std::enable_if_exists_t<typename T::foo, bool>
test(int)
{
  return true;
}

template <typename T>
static constexpr bool
test(...)
{
  return false;
}

template <typename T>
static constexpr std::enable_if_exists_t<typename T::foo, bool>
test_t(int)
{
  return true;
}

template <typename T>
static constexpr bool
test_t(...)
{
  return false;
}

struct Foo
{
  using foo = int;
};

struct Bar
{};

static_assert(test<Foo>(0),    "enable_if_exists failed");
static_assert(!test<Bar>(0),   "enable_if_exists failed");
static_assert(test_t<Foo>(0),  "enable_if_exists failed");
static_assert(!test_t<Bar>(0), "enable_if_exists failed");

// remove_cv_reference
static_assert(
  std::is_same<std::remove_cv_reference<int const&>::type, int>::value,
  "remove_cv_reference failed");
static_assert(
  std::is_same<std::remove_cv_reference_t<int const&>, int>::value,
  "remove_cv_reference failed");

// identity
static_assert(
  std::is_same<std::identity<Foo>::type, Foo>::value,
  "identity failed");

namespace
{
  void get_signature()
  {
    // get_signature.
    auto my_strlen = [](const char*) { return 42; };
    static_assert(
      std::is_same<std::get_signature<decltype(my_strlen)>,
                   auto (const char*) -> int>::value,
      "get_signature failed");
    BOOST_CHECK_EQUAL(my_strlen("42"), 42);
  }
}


ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(get_signature));
}
