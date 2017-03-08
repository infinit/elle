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

  {
    auto n = Node{42, {}};
    n.age.start();
    ELLE_LOG("n: \"%s\"", n);

    auto s1 = serialize(n);
    ELLE_LOG("s1: \"%s\"", s1);
    {
      auto const n1 = deserialize<Node>(s1);
      ELLE_LOG("n1: \"%s\"", n1);
      BOOST_TEST(n.age == n1.age);
    }

    /// Let time pass.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    auto s2 = serialize(n);
    ELLE_LOG("s2: \"%s\"", s2);

    /// Both serializations must differ, as time passed.
    BOOST_TEST(s1 != s2);
    {
      auto const n2 = deserialize<Node>(s2);
      ELLE_LOG("n2: \"%s\"", n2);
      BOOST_TEST(n.age == n2.age,
                 tt::tolerance(2.0f));
    }
  }
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(compare), 0, 3);
  suite.add(BOOST_TEST_CASE(serialization), 0, 3);
}
