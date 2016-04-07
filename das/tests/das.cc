#include <elle/UUID.hh>
#include <elle/serialization/json.hh>
#include <elle/test.hh>
#include <elle/optional.hh>

#include <das/model.hh>

struct serialization
{
  static elle::Version version;
};
elle::Version serialization::version(0, 0, 0);

class Device
{
public:
  das::Variable<std::string> name;
  boost::optional<std::string> model;
  elle::UUID id;
  Device(std::string name_,
         boost::optional<std::string> model_ = boost::none,
         elle::UUID id_ = elle::UUID::random())
    : name(std::move(name_))
    , model(std::move(model_))
    , id(std::move(id_))
  {}

  Device()
    : name()
    , model()
    , id()
  {}

  bool
  operator ==(Device const& other) const
  {
    return this->id == other.id && this->name == other.name;
  }

  typedef serialization serialization_tag;
};

DAS_MODEL_FIELD(Device, name);
DAS_MODEL_FIELD(Device, model);
DAS_MODEL_FIELD(Device, id);

using das::operator <<;

typedef das::Object<
  Device,
  das::Field<Device, elle::UUID, &Device::id>,
  das::Field<Device, das::Variable<std::string>, &Device::name>,
  das::Field<Device, boost::optional<std::string>, &Device::model>
  >
DasDevice;

DAS_MODEL_DEFAULT(Device, DasDevice);

static
void
printer()
{
  BOOST_CHECK_EQUAL(
    elle::sprintf("%s", Device("name", std::string("model"), elle::UUID())),
    "Device(id = 00000000-0000-0000-0000-000000000000, name = name, model = model)");
}

static
void
object_update()
{
  auto id = elle::UUID::random();
  Device d("device-name", std::string("model"), id);
  {
    DasDevice::Update u;
    u.apply(d);
    BOOST_CHECK_EQUAL(d.name.value(), "device-name");
    BOOST_CHECK_EQUAL(d.id, id);
    BOOST_CHECK_EQUAL(d.model.get(), "model");
  }
  {
    DasDevice::Update u;
    u.name = "new-name";
    u.apply(d);
    BOOST_CHECK_EQUAL(d.name.value(), "new-name");
    BOOST_CHECK_EQUAL(d.id, id);
    BOOST_CHECK_EQUAL(d.model.get(), "model");
  }
}

class User
{
public:
  std::string name;
  Device device;

  typedef serialization serialization_tag;
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

class Model
{
public:
  std::vector<Device> devices;
  typedef serialization serialization_tag;
};

DAS_MODEL_FIELD(Model, devices);

typedef das::Object<
  Model,
  das::Field<Model, std::vector<Device>, &Model::devices, DasDevices>
  > DasModel;

static
void
collection_composite()
{
  DasModel::Update u;
  u.devices.emplace();
  {
    DasDevice::Update du;
    du.id = elle::UUID::random();
    du.name = "name";
    u.devices.get().push_back(std::move(du));
  }
  Model m;
  u.apply(m);
  BOOST_CHECK_EQUAL(m.devices.size(), 1u);
  BOOST_CHECK_EQUAL(m.devices[0].name.value(), "name");
}

static
void
variable()
{
  das::Variable<int> v1(0);
  bool changed = false;
  v1.changed().connect(
    [&] (int const&) {BOOST_CHECK(!changed); changed = true;});
  auto check_changed = [&] {BOOST_CHECK(changed); changed = false;};
  v1 = 1;
  check_changed();
  BOOST_CHECK_EQUAL(v1, 1);
  v1 = das::Variable<int>(2);
  check_changed();
  BOOST_CHECK_EQUAL(v1, 2);
  das::Variable<int> moved = std::move(v1);
  moved = 3;
  check_changed();
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
  auto check_added = [&] { BOOST_CHECK(added); added = boost::none; };
  auto check_reset = [&] { BOOST_CHECK(reset); reset = false; };
  auto check_removed = [&] { BOOST_CHECK(removed); removed = boost::none; };
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
  l.reset(std::vector<Device>{d1, d2});
  check_changed();
  check_reset();
}

static
void
update_print()
{
  DasDevice::Update u;
  BOOST_CHECK_EQUAL(elle::sprintf("%s", u), "Device::Update()");
  auto id = elle::UUID::random();
  u.id = id;
  BOOST_CHECK_EQUAL(elle::sprintf("%s", u),
                    elle::sprintf("Device::Update(id = %s)", id));
  u.name = "COIN";
  BOOST_CHECK_EQUAL(elle::sprintf("%s", u),
                    elle::sprintf("Device::Update(id = %s, name = COIN)", id));
  DasDevices::Update lu;
  lu.push_back(std::move(u));
  lu.push_back(DasDevice::Update());
  BOOST_CHECK_EQUAL(
    elle::sprintf("%s", lu),
    elle::sprintf("[Device::Update(id = %s, name = COIN),"
                  " Device::Update()]", id));
}

static
void
serialization()
{
  {
    std::stringstream data("{}");
    elle::serialization::json::SerializerIn input(data, false);
    DasDevice::Update u(input);
    BOOST_CHECK(!u.id);
    BOOST_CHECK(!u.name);
  }
  // Load model from JSON.
  {
    std::stringstream data(
      elle::sprintf(
        "{"
        "  \"devices\": ["
        "    {"
        "      \"id\": \"%s\","
        "      \"name\": \"das-device\""
        "    }"
        "  ]"
        "}",
        elle::UUID::random()
        ));
    elle::serialization::json::SerializerIn input(data, false);
    DasModel::Update u(input);
    BOOST_CHECK_EQUAL(u.devices.get().size(), 1u);
    BOOST_CHECK_EQUAL(u.devices.get()[0].name.value(), "das-device");
  }
  // Remove device from model using JSON.
  {
    std::vector<Device> devices;
    {
      DasDevices::Update u;
      u.apply(devices);
      BOOST_CHECK(devices.empty());
    }
    auto device_id = elle::UUID::random();
    {
      DasDevices::Update u;
      DasDevice::Update du;
      du.id = device_id;
      du.name = "device-name-1";
      u.push_back(std::move(du));
      u.apply(devices);
      BOOST_CHECK_EQUAL(devices.size(), 1u);
      BOOST_CHECK_EQUAL(devices[0].id, device_id);
      BOOST_CHECK_EQUAL(devices[0].name.value(), "device-name-1");
    }
    {
      auto update_device = [&] (bool remove)
      {
        DasDevices::Update u;
        std::stringstream data(
          elle::sprintf(
            "{"
            "  \"$remove\": %s,"
            "  \"id\": \"%s\""
            "}",
            remove ? "true" : "false", device_id
          ));
        elle::serialization::json::SerializerIn input(data, false);
        DasDevices::ElementUpdate du(input);
        u.push_back(std::move(du));
        u.apply(devices);
      };
      update_device(false);
      BOOST_CHECK_EQUAL(devices.size(), 1u);
      update_device(true);
      BOOST_CHECK_EQUAL(devices.size(), 0u);
    }
  }
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(object_update), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(object_composite), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(collection), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(collection_composite), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(variable), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(index_list), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(update_print), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(serialization), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(printer), 0, valgrind(1));
}
