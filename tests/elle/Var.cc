#include <elle/Var.hh>

#include <elle/test.hh>

static
void
pod()
{
  elle::Var<int> var(0);
  int count = 0;
  int value = 0;
  var.on_update(
    [&] (int v)
    {
      ++count;
      BOOST_TEST(var == v);
      value = v;
    });
  var = 2;
  BOOST_TEST(value == 2);
  var += 2;
  BOOST_TEST(value == 4);
  BOOST_TEST(count == 2);
}

static
void
object()
{
  elle::Var<std::string> var("iable");
  int count = 0;
  std::string value = "iable";
  var.on_update(
    [&] (std::string v)
    {
      ++count;
      BOOST_TEST(var == v);
      value = v;
    });
  var = "ice";
  BOOST_TEST(value == "ice");
  var += "lle";
  BOOST_TEST(value == "icelle");
  BOOST_TEST(value.length() == 6);
  BOOST_TEST(count == 2);
}

ELLE_TEST_SUITE()
{
  auto& master = boost::unit_test::framework::master_test_suite();
  master.add(BOOST_TEST_CASE(pod));
  master.add(BOOST_TEST_CASE(object));
}
