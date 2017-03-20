#include <string>

#include <elle/json/json.hh>
#include <elle/serialization/Serializer.hh>
#include <elle/serialization/json.hh>
#include <elle/test.hh>

#include <elle/das/printer.hh>
#include <elle/das/serializer.hh>
#include <elle/das/Symbol.hh>

ELLE_LOG_COMPONENT("das.serializer.test");

namespace symbol
{
  ELLE_DAS_SYMBOL(device);
  ELLE_DAS_SYMBOL(id);
  ELLE_DAS_SYMBOL(name);
}

using elle::das::operator <<;

/// A struct without ctor.
struct DevicePOD
{
  bool
  operator ==(DevicePOD const& rhs) const
  {
    return this->id == rhs.id && this->name == rhs.name;
  }

  int id;
  std::string name;
  int useless_and_not_serialized;

  using Model
    = elle::das::Model<DevicePOD,
                       decltype(elle::meta::list(symbol::id,
                                                 symbol::name))>;
};

struct NopeString
  : public std::string
{
  using Super = std::string;
  NopeString()
  {}

  NopeString(std::string const& s)
    : Super{s}
  {}

  NopeString(char const* s)
    : Super{s}
  {}

  NopeString
  operator =(NopeString const&) = delete;
};

/// A struct with a ctor.
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
  int useless_and_not_serialized;

  using Model = elle::das::Model<Device,
                                 decltype(elle::meta::list(symbol::id,
                                                           symbol::name))>;
};

struct User
{
  User(std::string name, std::vector<Device> d = {})
    : name(std::move(name))
    , device(std::move(d))
  {}

  std::string name;
  std::vector<Device> device;

  bool
  operator ==(User const& rhs) const
  {
    return this->name == rhs.name && this->device == rhs.device;
  }

  using Model = elle::das::Model<User,
                                 decltype(elle::meta::list(symbol::name,
                                                           symbol::device))>;
};

ELLE_DAS_SERIALIZE(DevicePOD);
ELLE_DAS_SERIALIZE(Device);
ELLE_DAS_SERIALIZE(User);

static
void
simple()
{
  using elle::serialization::json::serialize;
  using elle::serialization::json::deserialize;
  ELLE_LOG("POD serialization");
  {
    auto const d = DevicePOD{42, "towel"};
    std::stringstream ss;
    serialize(d, ss);
    ELLE_LOG("serialized: \"%s\"", ss.str());
    BOOST_CHECK_EQUAL(deserialize<DevicePOD>(ss), d);
  }
  ELLE_LOG("Object serialization");
  {
    auto const d = Device{42, "towel"};
    std::stringstream ss;
    serialize(d, ss);
    ELLE_LOG("serialized: \"%s\"", ss.str());
    BOOST_CHECK_EQUAL(deserialize<Device>(ss), d);
  }
}

static
void
composite()
{
  using elle::serialization::json::serialize;
  using elle::serialization::json::deserialize;

  auto const u = User("Doug", {Device(42, "arthur"), Device(51, "ford")});
  using Model = elle::das::Model<User,
                                 decltype(elle::meta::list(symbol::name))>;
  using S = elle::das::Serializer<User, Model>;
  ELLE_LOG("serialize with default model")
  {
    std::stringstream ss;
    serialize(u, ss);
    BOOST_CHECK_EQUAL(deserialize<User>(ss), u);
  }
  ELLE_LOG("serialize with custom model")
  {
    std::stringstream ss;
    serialize<S>(u, ss);
    BOOST_CHECK_EQUAL((deserialize<User, S>(ss)), User("Doug"));
  }
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(simple), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(composite), 0, valgrind(1));
}
