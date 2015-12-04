#include <elle/TypeInfo.hh>
#include <elle/test.hh>
#include <elle/log.hh>

static
void
comparison()
{
  BOOST_CHECK_EQUAL(elle::type_info<int>(), elle::type_info<int>());
  BOOST_CHECK_NE(elle::type_info<int>(), elle::type_info<float>());
}

static
size_t
h(elle::TypeInfo const& info)
{
  return std::hash<elle::TypeInfo>()(info);
}

static
void
hash()
{
  BOOST_CHECK_EQUAL(h(elle::type_info<int>()), h(elle::type_info<int>()));
  BOOST_CHECK_NE(h(elle::type_info<int>()), h(elle::type_info<float>()));
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(comparison));
  suite.add(BOOST_TEST_CASE(hash));
}
