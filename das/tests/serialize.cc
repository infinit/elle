#include <string>

#include <elle/json/json.hh>
#include <elle/serialization/Serializer.hh>
#include <elle/serialization/json.hh>
#include <elle/test.hh>

#include <das/printer.hh>
#include <das/serializer.hh>
#include <das/Symbol.hh>

ELLE_LOG_COMPONENT("das.serializer.test");

DAS_SYMBOL(device);
DAS_SYMBOL(id);
DAS_SYMBOL(name);

using das::operator <<;

struct DevicePOD
{
  bool
  operator ==(DevicePOD const& rhs) const
  {
    return this->id == rhs.id && this->name == rhs.name;
  }

  int id;
  std::string name;

  using Model = das::Model<DevicePOD, elle::meta::List<Symbol_id, Symbol_name>>;
};

struct Device
  : public DevicePOD
{
  Device(int id, std::string name)
  {
    this->id = id;
    this->name = name;
  }
};

struct User
{
  User(std::string name, std::vector<Device> d = {})
    : name(name)
    , device(std::move(d))
  {}

  std::string name;
  std::vector<Device> device;

  bool
  operator ==(User const& rhs) const
  {
    return this->name == rhs.name && this->device == rhs.device;
  }

  using Model = das::Model<User, elle::meta::List<Symbol_name, Symbol_device>>;
};

namespace elle
{
  namespace serialization
  {
    template <>
    struct Serialize<DevicePOD>
      : public das::Serializer<DevicePOD>
    {};

    template <>
    struct Serialize<Device>
      : public das::Serializer<Device>
    {};

    template <>
    struct Serialize<User>
      : public das::Serializer<User>
    {};
  }
}

static
void
simple()
{
  {
    DevicePOD d;
    d.id = 42;
    d.name = "towel";
    std::stringstream ss;
    elle::serialization::json::serialize(d, ss);
    BOOST_CHECK_EQUAL(
      elle::serialization::json::deserialize<DevicePOD>(ss), d);
  }
  {
    Device d(42, "towel");
    std::stringstream ss;
    elle::serialization::json::serialize(d, ss);
    BOOST_CHECK_EQUAL(
      elle::serialization::json::deserialize<Device>(ss), d);
  }
}

static
void
composite()
{
  User u("Doug", {Device(42, "arthur"), Device(51, "ford")});
  using Model = das::Model<User, elle::meta::List<Symbol_name>>;
  using S = das::Serializer<User, Model>;
  ELLE_LOG("serialize with default model")
  {
    std::stringstream ss;
    elle::serialization::json::serialize(u, ss);
    BOOST_CHECK_EQUAL(
      elle::serialization::json::deserialize<User>(ss), u);
  }
  ELLE_LOG("serialize with custom model")
  {
    std::stringstream ss;
    elle::serialization::json::serialize<S>(u, ss);
    BOOST_CHECK_EQUAL(
      (elle::serialization::json::deserialize<User, S>(ss)), User("Doug"));
  }
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(simple), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(composite), 0, valgrind(1));
}
