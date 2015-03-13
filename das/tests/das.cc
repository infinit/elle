#include <elle/UUID.hh>
#include <elle/serialization/json.hh>
#include <elle/test.hh>

#include <das/model.hh>

class Device
{
public:
  das::Variable<std::string> name;
  elle::UUID id;
  void
  serialize(elle::serialization::Serializer& s)
  {
    s.serialize("name", this->name);
  }
};

namespace das
{
  template <>
  class Field<Device, das::Variable<std::string>, &Device::name>
  {
  public:
    constexpr static char const* _name = "name";
  };
}

static
void
simple()
{
  std::stringstream data("{}");
  elle::serialization::json::SerializerIn input(data);
  Device d;
  typedef das::Object<
    Device,
    das::Field<Device, das::Variable<std::string>, &Device::name>
    >
    DasDevice;
  DasDevice::Update u(input);
  u.apply(d);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(simple), 0, valgrind(1));
}
