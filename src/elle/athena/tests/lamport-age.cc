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
  bool
  operator ==(Self const& rhs) const
  {
    return this->id == rhs.id && this->age == rhs.age;
  }

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
serialization()
{
  namespace tt = boost::test_tools;
  using elle::serialization::json::serialize;
  using elle::serialization::json::deserialize;
  {
    auto const n = Node{42, 1943};
    ELLE_LOG("n: \"%s\"", n);

    auto s1 = serialize(n);
    ELLE_LOG("s1: \"%s\"", s1);
    {
      auto const n1 = deserialize<Node>(s1);
      ELLE_LOG("n1: \"%s\"", n1);
      BOOST_TEST(float(n.age.timestamp()) == float(n1.age.timestamp()), tt::tolerance(2.0f));
    }

    /// Let time pass.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    auto s2 = serialize(n);
    ELLE_LOG("s2: \"%s\"", s2);

    /// Both serializations must differ, as time passed.
    BOOST_TEST(s1 != s2);
    {
      auto const n2 = deserialize<Node>(s2);
      ELLE_LOG("n2: \"%s\"", n2);
      BOOST_TEST(float(n.age.timestamp()) == float(n2.age.timestamp()), tt::tolerance(2.0f));
    }
  }
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(serialization), 0, valgrind(1));
}
