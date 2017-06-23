#include <elle/TypeInfo.hh>
#include <elle/test.hh>
#include <elle/log.hh>

// Avoid unnamed namespace qualifications.
namespace foo
{
  struct Foo
  {
    struct Bar
    {
      struct Baz
      {
        struct Qux {};
      };
    };
  };
}

namespace
{
  void
  comparison()
  {
    BOOST_TEST(elle::type_info<int>() == elle::type_info<int>());
    BOOST_TEST(elle::type_info<int>() != elle::type_info<float>());
  }

  size_t
  h(elle::TypeInfo const& info)
  {
    return std::hash<elle::TypeInfo>()(info);
  }

  void
  hash()
  {
    BOOST_TEST(h(elle::type_info<int>()) == h(elle::type_info<int>()));
    BOOST_TEST(h(elle::type_info<int>()) != h(elle::type_info<float>()));
  }

  ELLE_TYPE_INFO_ABBR("FB", "foo::Foo::Bar");
  ELLE_TYPE_INFO_ABBR("FBB", "foo::Foo::Bar::Baz");

  /// The abbreviated name of T.
  template <typename T>
  std::string
  name()
  {
    return elle::sprintf("%f", elle::type_info<T>());
  }

  void
  abbreviations()
  {
    BOOST_TEST(name<foo::Foo>() == "foo::Foo");
    BOOST_TEST(name<foo::Foo::Bar>() == "FB");
    BOOST_TEST(name<foo::Foo::Bar::Baz>() == "FBB");
    BOOST_TEST(name<foo::Foo::Bar::Baz::Qux>() == "FBB::Qux");
  }
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(comparison));
  suite.add(BOOST_TEST_CASE(hash));
  suite.add(BOOST_TEST_CASE(abbreviations));
}
