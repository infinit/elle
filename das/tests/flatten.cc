#include <string>

#include <elle/test.hh>

#include <das/flatten.hh>
#include <das/Symbol.hh>

DAS_SYMBOL(device);
DAS_SYMBOL(id);
DAS_SYMBOL(name);

struct Device
{
  Device(int id, std::string name)
    : id(id)
    , name(name)
  {}

  int id;
  std::string name;

  using Model = das::Model<Device, elle::meta::List<Symbol_id, Symbol_name>>;
};

static
void
value()
{
  Device d(42, "towel");
  auto flat = das::flatten(d);
  BOOST_CHECK_EQUAL(
    flat,
    std::make_tuple(42, "towel"));
}

static
void
ref()
{
  Device d(42, "towel");
  auto flat = das::flatten_ref(d);
  BOOST_CHECK_EQUAL(std::get<0>(flat), 42);
  BOOST_CHECK_EQUAL(std::get<1>(flat), "towel");
  std::get<0>(flat) = 51;
  BOOST_CHECK_EQUAL(d.id, 51);
  d.id = 69;
  BOOST_CHECK_EQUAL(std::get<0>(flat), 69);
}

static
void
cref()
{
  Device d(42, "towel");
  auto flat = das::flatten_ref(static_cast<Device const&>(d));
  BOOST_CHECK_EQUAL(std::get<0>(flat), 42);
  BOOST_CHECK_EQUAL(std::get<1>(flat), "towel");
  d.id = 51;
  BOOST_CHECK_EQUAL(std::get<0>(flat), 51);
}

struct User
{
  User(std::string name, Device d)
    : name(name)
    , device(std::move(d))
  {}

  std::string name;
  Device device;

  using Model = das::Model<User, elle::meta::List<Symbol_name, Symbol_device>>;
};

static
void
composite()
{
  User u("Doug", Device(42, "towel"));
  auto flat = das::flatten_ref(u);
  BOOST_CHECK_EQUAL(std::get<0>(flat), "Doug");
  BOOST_CHECK_EQUAL(std::get<0>(std::get<1>(flat)), 42);
  BOOST_CHECK_EQUAL(std::get<1>(std::get<1>(flat)), "towel");
  std::get<1>(std::get<1>(flat)) = "traveler";
  BOOST_CHECK_EQUAL(u.device.name, "traveler");
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(value), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(ref), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(cref), 0, valgrind(1));
  suite.add(BOOST_TEST_CASE(composite), 0, valgrind(1));
}
