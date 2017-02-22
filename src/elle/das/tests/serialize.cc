#include <string>

#include <elle/json/json.hh>
#include <elle/serialization/Serializer.hh>
#include <elle/serialization/json.hh>
#include <elle/test.hh>

#include <elle/das/printer.hh>
#include <elle/das/serializer.hh>
#include <elle/das/Symbol.hh>

ELLE_LOG_COMPONENT("das.serializer.test");

ELLE_DAS_SYMBOL(device);
ELLE_DAS_SYMBOL(id);
ELLE_DAS_SYMBOL(name);

using elle::das::operator <<;

struct DevicePOD
{
  bool
  operator ==(DevicePOD const& rhs) const
  {
    return this->id == rhs.id && this->name == rhs.name;
  }

  int id;
  std::string name;

  using Model = elle::das::Model<DevicePOD, elle::meta::List<Symbol_id,
                                                             Symbol_name>>;
};

struct NopeString
  : public std::string
{
  NopeString()
  {}

  NopeString(std::string const& s)
    : std::string(s)
  {}

  NopeString(char const* s)
    : std::string(s)
  {}

  NopeString
  operator =(NopeString const&) = delete;
};

struct Device
{
  Device(int id, NopeString name)
    : id(id)
    , name(name)
  {}

  bool
  operator ==(Device const& rhs) const
  {
    return this->id == rhs.id && this->name == rhs.name;
  }

  int id;
  NopeString name;

  using Model = elle::das::Model<Device, elle::meta::List<Symbol_id,
                                                          Symbol_name>>;
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

  using Model = elle::das::Model<User, elle::meta::List<Symbol_name,
                                                        Symbol_device>>;
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
  ELLE_LOG("POD serialization");
  {
    DevicePOD d;
    d.id = 42;
    d.name = "towel";
    std::stringstream ss;
    elle::serialization::json::serialize(d, ss);
    ELLE_LOG("serialized: \"%s\"", ss.str());
    BOOST_CHECK_EQUAL(
      elle::serialization::json::deserialize<DevicePOD>(ss), d);
  }
  ELLE_LOG("Object serialization");
  {
    Device d(42, "towel");
    std::stringstream ss;
    elle::serialization::json::serialize(d, ss);
    ELLE_LOG("serialized: \"%s\"", ss.str());
    BOOST_CHECK_EQUAL(
      elle::serialization::json::deserialize<Device>(ss), d);
  }
}

static
void
composite()
{
  User u("Doug", {Device(42, "arthur"), Device(51, "ford")});
  using Model = elle::das::Model<User, elle::meta::List<Symbol_name>>;
  using S = elle::das::Serializer<User, Model>;
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
