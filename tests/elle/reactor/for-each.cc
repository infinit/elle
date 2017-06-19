#include <elle/log.hh>
#include <elle/test.hh>

#include "elle/reactor/for-each.hh"

ELLE_LOG_COMPONENT("elle.reactor.for-each.test");

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

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(empty));
  master.add(BOOST_TEST_CASE(const_not_copiable));
  master.add(BOOST_TEST_CASE(mutable_not_copiable));
  // master.add(BOOST_TEST_CASE(moved_not_copiable));
}
