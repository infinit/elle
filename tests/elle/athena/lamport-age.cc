#include <chrono>
#include <string>
#include <thread>

#include <elle/json/json.hh>
#include <elle/serialization/Serializer.hh>
#include <elle/serialization/json.hh>
#include <elle/test.hh>

#include <elle/athena/LamportAge.hh>
#include <elle/das/Symbol.hh>
#include <elle/das/printer.hh>
#include <elle/das/serializer.hh>

ELLE_LOG_COMPONENT("athena.lamport-age.test");

namespace symbol
{
  ELLE_DAS_SYMBOL(age);
  ELLE_DAS_SYMBOL(id);
}

using elle::das::operator <<;

struct Node
 {
  using Self = Node;

  int id;
  elle::athena::LamportAge age;

  using Model
    = elle::das::Model<Self,
                       decltype(elle::meta::list(symbol::id,
                                                 symbol::age))>;
};

ELLE_DAS_SERIALIZE(Node);

static
void
compare()
{
  using elle::athena::LamportAge;
  auto a1 = LamportAge{};
  BOOST_TEST(a1 == a1);
  auto a2 = a1;
  a2.start();
  BOOST_TEST(a1 < a2);
}


static
void
serialization()
{
  namespace tt = boost::test_tools;
  using elle::serialization::json::serialize;
  using elle::serialization::json::deserialize;

#define CHECK_EQUAL_AGE(Age1, Age2)                     \
  do {                                                  \
    ELLE_LOG("Check %s == %s", Age1, Age2);             \
    ELLE_LOG("Check %s == %s", Age1.age(), Age2.age()); \
    auto min = std::min(Age1.age(), Age2.age());        \
    auto max = std::max(Age1.age(), Age2.age());        \
    auto d = max - min;                                 \
    BOOST_TEST(d <= std::chrono::seconds(3));           \
  } while (false)


  {
    // n0's age will not change, we don't `start` it.
    auto const n0 = Node{21, {}};
    ELLE_LOG("n0: \"%s\"", n0);
    auto s01 = serialize(n0);
    BOOST_TEST(n0.age == deserialize<Node>(s01).age);

    // n will be aging.
    auto n = Node{42, {}};
    n.age.start();
    ELLE_LOG("n: \"%s\"", n);

    auto s1 = serialize(n);
    ELLE_LOG("s1: \"%s\"", s1);
    {
      auto const n1 = deserialize<Node>(s1);
      ELLE_LOG("n1: \"%s\"", n1);
      CHECK_EQUAL_AGE(n.age, n1.age);
    }

    /// Let time pass.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    auto s2 = serialize(n);
    ELLE_LOG("s2: \"%s\"", s2);

    /// n0 was not started, the serialization is the same.
    {
      auto s02 = serialize(n0);
      BOOST_TEST(s01 == s02);
      CHECK_EQUAL_AGE(n0.age, deserialize<Node>(s02).age);
    }

    /// Both serializations must differ, as time passed.
    BOOST_TEST(s1 != s2);
    {
      auto const n2 = deserialize<Node>(s2);
      ELLE_LOG("n2: \"%s\"", n2);
      CHECK_EQUAL_AGE(n.age, n2.age);
    }
  }
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(compare), 0, 3);
  suite.add(BOOST_TEST_CASE(serialization), 0, 3);
}
