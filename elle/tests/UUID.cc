#include <boost/uuid/random_generator.hpp>

#include <elle/UUID.hh>
#include <elle/serialization/json.hh>
#include <elle/test.hh>

namespace uuid
{
  static
  void
  nil()
  {
    BOOST_CHECK_EQUAL(elle::UUID(),
                      elle::UUID("00000000-0000-0000-0000-000000000000"));
    BOOST_CHECK(elle::UUID().is_nil());
  }

  static
  void
  from_boost()
  {
    auto uuid = boost::uuids::random_generator()();
    BOOST_CHECK_EQUAL(elle::UUID(uuid), uuid);
  }

  static
  void
  from_string()
  {
    auto uuid = boost::uuids::random_generator()();
    BOOST_CHECK_EQUAL(elle::UUID(boost::lexical_cast<std::string>(uuid)), uuid);
    BOOST_CHECK_THROW(elle::UUID("coin pan"), elle::InvalidUUID);
  }

  static
  void
  random()
  {
    BOOST_CHECK_NE(elle::UUID::random(), elle::UUID::random());
    BOOST_CHECK(!elle::UUID::random().is_nil());
  }

  static
  void
  repr()
  {
    auto uuid = boost::uuids::random_generator()();
    BOOST_CHECK_EQUAL(elle::UUID(uuid).repr(),
                      boost::lexical_cast<std::string>(uuid));
  }

  static
  void
  serialization()
  {
    auto uuid = boost::uuids::random_generator()();
    std::string json = R"JSON(
    {
      "empty": "",
      "nil": "00000000-0000-0000-0000-000000000000",
      "random": "%s",
      "invalid": "garbage"
    }
    )JSON";
    json = elle::sprintf(json, uuid);
    std::stringstream input(json);
    elle::serialization::json::SerializerIn s(input);
    elle::UUID v;
    s.serialize("empty", v);
    BOOST_CHECK(v.is_nil());
    s.serialize("nil", v);
    BOOST_CHECK(v.is_nil());
    s.serialize("random", v);
    BOOST_CHECK_EQUAL(v, uuid);
    BOOST_CHECK_THROW(s.serialize("invalid", v), elle::InvalidUUID);
  }
}

ELLE_TEST_SUITE()
{
  using namespace uuid;
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(nil));
  master.add(BOOST_TEST_CASE(from_boost));
  master.add(BOOST_TEST_CASE(from_string));
  auto random = &uuid::random;
  master.add(BOOST_TEST_CASE(random));
  master.add(BOOST_TEST_CASE(repr));
  master.add(BOOST_TEST_CASE(serialization));
}
