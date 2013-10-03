#define BOOST_TEST_MODULE memory

#include <boost/test/unit_test.hpp>
#include <elle/memory.hh>
#include <tuple>

BOOST_AUTO_TEST_CASE(make_unique)
{
  std::unique_ptr<std::tuple<int, char, double>> ptr =
    elle::make_unique<std::tuple<int, char, double>>(1, 'c', 1.0f);
  BOOST_CHECK(std::get<0>(*ptr) == 1);
  BOOST_CHECK(std::get<1>(*ptr) == 'c');
  BOOST_CHECK(std::get<2>(*ptr) == 1.0f);

  std::unique_ptr<std::tuple<>> empty_ptr =
    elle::make_unique<std::tuple<>>();
}

BOOST_AUTO_TEST_CASE(generic_unique_ptr)
{
  elle::generic_unique_ptr<int> empty();
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
