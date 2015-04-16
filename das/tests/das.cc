#include <elle/UUID.hh>
#include <elle/serialization/json.hh>
#include <elle/test.hh>

#include <das/model.hh>

class Device
  : public elle::Printable
{
public:
  das::Variable<std::string> name;
  elle::UUID id;
  Device(std::string name_, elle::UUID id_ = elle::UUID::random())
    : name(std::move(name_))
    , id(std::move(id_))
  {}

  Device()
    : name()
    , id()
  {}

  bool
  operator ==(Device const& other) const
  {
    return this->id == other.id && this->name == other.name;
  }

  virtual
  void
  print(std::ostream& s) const
  {
    elle::fprintf(s, "Device(%s, \"%s\")", this->id, this->name);
  }
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

class User
{
public:
  std::string name;
  Device device;
};

DAS_MODEL_FIELD(User, name);
DAS_MODEL_FIELD(User, device);

typedef das::Object<
  User,
  das::Field<User, std::string, &User::name>,
  das::Field<User, Device, &User::device, DasDevice>
  >
DasUser;

static
void
object_composite()
{
  User user;
  user.name = "user-name";
  user.device.name = "device-name";
  {
    DasUser::Update u;
    u.name = "new-user-name";
    u.apply(user);
    BOOST_CHECK_EQUAL(user.name, "new-user-name");
    BOOST_CHECK_EQUAL(user.device.name.value(), "device-name");
    u.device = DasDevice::Update();
    u.apply(user);
    BOOST_CHECK_EQUAL(user.name, "new-user-name");
    BOOST_CHECK_EQUAL(user.device.name.value(), "device-name");
    u.device.get().name = "new-device-name";
    u.apply(user);
    BOOST_CHECK_EQUAL(user.name, "new-user-name");
    BOOST_CHECK_EQUAL(user.device.name.value(), "new-device-name");
  }
}

typedef das::Collection<Device, elle::UUID, &Device::id, DasDevice> DasDevices;

static
void
collection()
{
  std::vector<Device> devices;
  {
    DasDevices::Update u;
    u.apply(devices);
    BOOST_CHECK(devices.empty());
  }
  {
    DasDevices::Update u;
    u.push_back(DasDevice::Update());
    BOOST_CHECK_THROW(u.apply(devices), elle::Error);
  }
  {
    DasDevices::Update u;
    DasDevice::Update du;
    auto id = elle::UUID::random();
    du.id = id;
    du.name = "device-name-1";
    u.push_back(std::move(du));
    u.apply(devices);
    BOOST_CHECK_EQUAL(devices.size(), 1u);
    BOOST_CHECK_EQUAL(devices[0].id, id);
    BOOST_CHECK_EQUAL(devices[0].name.value(), "device-name-1");
  }
  {
    DasDevices::Update u;
    {
      DasDevice::Update du;
      auto id = elle::UUID::random();
      du.id = id;
      du.name = "device-name-2";
      u.push_back(std::move(du));
    }
    {
      DasDevice::Update du;
      du.id = devices[0].id;
      du.name = "new-device-name-1";
      u.push_back(std::move(du));
    }
    u.apply(devices);
    BOOST_CHECK_EQUAL(devices.size(), 2u);
    BOOST_CHECK_EQUAL(devices[0].name.value(), "new-device-name-1");
    BOOST_CHECK_EQUAL(devices[1].name.value(), "device-name-2");
  }
  {
    DasDevices::Update u;
    {
      DasDevices::ElementUpdate du;
      du.id = devices[0].id;
      du.remove = true;
      u.push_back(std::move(du));
    }
    u.apply(devices);
    BOOST_CHECK_EQUAL(devices.size(), 1u);
    BOOST_CHECK_EQUAL(devices[0].name.value(), "device-name-2");
  }
}

static
void
index_list()
{
  bool changed = false;
  bool reset = false;
  boost::optional<elle::UUID> added;
  boost::optional<elle::UUID> removed;
  das::IndexList<Device, elle::UUID, &Device::id> l;
  l.changed().connect([&] {BOOST_CHECK(!changed); changed = true;});
  l.reset().connect([&] {BOOST_CHECK(!reset); reset = true;});
  l.added().connect([&] (Device& d){BOOST_CHECK(!added); added = d.id;});
  l.removed().connect(
    [&] (elle::UUID const& id){BOOST_CHECK(!removed); removed = id;});
  auto check_changed = [&] { BOOST_CHECK(changed); changed = false; };
  auto check_added = [&] { BOOST_CHECK(added); added = {}; };
  auto check_removed = [&] { BOOST_CHECK(removed); removed = {}; };
  Device d1("d1");
  Device d2("d1");
  l.add(d1);
  check_changed();
  check_added();
  l.add(d2);
  check_changed();
  check_added();
  BOOST_CHECK_EQUAL(l.size(), 2u);
  BOOST_CHECK_EQUAL(l.get(d1.id), d1);
  BOOST_CHECK_EQUAL(l.get(d2.id), d2);
  BOOST_CHECK_THROW(l.get(elle::UUID::random()), elle::Error);
  BOOST_CHECK_THROW(l.add(d1), elle::Error);
  l.remove(d2.id);
  check_changed();
  check_removed();
  BOOST_CHECK_EQUAL(l.size(), 1u);
  BOOST_CHECK_THROW(l.get(d2.id), elle::Error);
  BOOST_CHECK_THROW(l.remove(d2.id), elle::Error);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(object_update), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(object_update_serialization), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(object_composite), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(collection), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(index_list), 0, valgrind(1));
}
