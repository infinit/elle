#include <elle/UUID.hh>
#include <elle/serialization/json.hh>
#include <elle/test.hh>

#include <das/model.hh>

class Device
{
public:
  das::Variable<std::string> name;
  elle::UUID id;
  Device(std::string name_, elle::UUID id_)
    : name(std::move(name_))
    , id(std::move(id_))
  {}
};

DAS_MODEL_FIELD(Device, name);
DAS_MODEL_FIELD(Device, id);

typedef das::Object<
  Device,
  das::Field<Device, elle::UUID, &Device::id>,
  das::Field<Device, das::Variable<std::string>, &Device::name>
  >
DasDevice;

static
void
object_update()
{
  auto id = elle::UUID::random();
  Device d("device-name", id);
  {
    DasDevice::Update u;
    u.apply(d);
    BOOST_CHECK_EQUAL(d.name.value(), "device-name");
    BOOST_CHECK_EQUAL(d.id, id);
  }
  {
    DasDevice::Update u;
    u.name = "new-name";
    u.apply(d);
    BOOST_CHECK_EQUAL(d.name.value(), "new-name");
    BOOST_CHECK_EQUAL(d.id, id);
  }
}

static
void
object_update_serialization()
{
  {
    std::stringstream data("{}");
    elle::serialization::json::SerializerIn input(data);
    DasDevice::Update u(input);
    BOOST_CHECK(!u.id);
    BOOST_CHECK(!u.name);
  }
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(object_update), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(object_update_serialization), 0, valgrind(1));
}
