#include <elle/log.hh>
#include <elle/test.hh>

#include "elle/reactor/for-each.hh"

ELLE_LOG_COMPONENT("elle.reactor.for-each.test");

ELLE_TEST_SCHEDULED(parallel)
{
  std::vector<int> c{0, 1, 2};
  elle::reactor::Barrier b;
  elle::reactor::Thread check(
    "check",
    [&]
    {
      BOOST_CHECK_EQUAL(c, std::vector<int>({0, 1, 2}));
      elle::reactor::yield();
      BOOST_CHECK_EQUAL(c, std::vector<int>({1, 2, 3}));
      elle::reactor::yield();
      b.open();
    });
  elle::reactor::for_each_parallel(
    c,
    [&] (int& c)
    {
      ++c;
      elle::reactor::wait(b);
    });
  BOOST_CHECK(b.opened());
}

ELLE_TEST_SCHEDULED(empty)
{
  elle::reactor::for_each_parallel(
    std::vector<int>(),
    [] (int i)
    {
      BOOST_FAIL("iterating over empty container");
    });
}

ELLE_TEST_SCHEDULED(const_not_copiable)
{
  std::vector<std::unique_ptr<int>> v;
  v.emplace_back(std::make_unique<int>(0));
  v.emplace_back(std::make_unique<int>(1));
  v.emplace_back(std::make_unique<int>(2));
  {
    auto check = 0;
    elle::reactor::for_each_parallel(
      v,
      [&check] (std::unique_ptr<int> const& i)
      {
        BOOST_TEST(*i == check++);
      });
  }
  {
    auto check = 0;
    for (auto const& i : v)
      BOOST_TEST(*i == check++);
  }
}

ELLE_TEST_SCHEDULED(mutable_not_copiable)
{
  std::vector<std::unique_ptr<int>> v;
  v.emplace_back(std::make_unique<int>(0));
  v.emplace_back(std::make_unique<int>(1));
  v.emplace_back(std::make_unique<int>(2));
  {
    auto check = 0;
    elle::reactor::for_each_parallel(
      v,
      [&check] (auto& i)
      {
        BOOST_TEST(*i == check++);
        i.reset();
      });
  }
  for (auto const& i : v)
    BOOST_TEST(!i);
}

ELLE_TEST_SCHEDULED(mutable_copiable)
{
  std::vector<std::shared_ptr<int>> v;
  v.emplace_back(std::make_shared<int>(0));
  v.emplace_back(std::make_shared<int>(1));
  v.emplace_back(std::make_shared<int>(2));
  {
    auto check = 0;
    elle::reactor::for_each_parallel(
      v,
      [&check] (auto i)
      {
        BOOST_TEST(*i == check++);
        i.reset();
      });
  }
  {
    auto check = 0;
    for (auto const& i : v)
      BOOST_TEST(*i == check++);
  }
}

ELLE_TEST_SCHEDULED(valued)
{
  std::vector<int> v{0, 1, 2};
  BOOST_TEST(
    elle::reactor::for_each_parallel(
      v, [] (auto i) { return std::to_string(i); }) ==
    (std::vector<std::string>{"0", "1", "2"}));
}

ELLE_TEST_SCHEDULED(valued_continue)
{
  std::vector<int> v{0, 1, 2, 3, 4, 5};
  BOOST_TEST(
    elle::reactor::for_each_parallel(
      v, [] (auto i) {
        if (i % 2)
          elle::reactor::continue_parallel();
        else
          return std::to_string(i);
      }) == (std::vector<std::string>{"0", "2", "4"}));
}

ELLE_TEST_SCHEDULED(parallel_break)
{
  std::vector<int> c{0, 1, 2};
  elle::reactor::for_each_parallel(
    c,
    [&] (int& c)
    {
      if (c == 1)
        elle::reactor::break_parallel();
      ++c;
    });
  BOOST_CHECK_EQUAL(c, std::vector<int>({1, 1, 2}));
}

// ELLE_TEST_SCHEDULED(moved_not_copiable)
// {
//   std::vector<std::unique_ptr<int>> v;
//   v.emplace_back(std::make_unique<int>(0));
//   v.emplace_back(std::make_unique<int>(1));
//   v.emplace_back(std::make_unique<int>(2));
//   {
//     auto check = 0;
//     elle::reactor::for_each_parallel(
//       std::move(v),
//       [&check] (std::unique_ptr<int> i)
//       {
//         BOOST_TEST(*i == check++);
//         i.reset();
//       });
//   }
// }

ELLE_TEST_SCHEDULED(initializer_list)
{
  auto res = 0;
  elle::reactor::for_each_parallel(
    {0, 1, 2},
    [&] (int i)
    {
      res += i;
    });
  BOOST_TEST(res == 3);
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(parallel));
  master.add(BOOST_TEST_CASE(empty));
  master.add(BOOST_TEST_CASE(const_not_copiable));
  master.add(BOOST_TEST_CASE(mutable_not_copiable));
  master.add(BOOST_TEST_CASE(mutable_copiable));
  master.add(BOOST_TEST_CASE(valued));
  master.add(BOOST_TEST_CASE(valued_continue));
  master.add(BOOST_TEST_CASE(parallel_break));
  // master.add(BOOST_TEST_CASE(moved_not_copiable));
  master.add(BOOST_TEST_CASE(initializer_list));
}
