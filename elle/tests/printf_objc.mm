#include <elle/printf.hh>
#include <elle/test.hh>

#include <Foundation/Foundation.h>

static
void
test_ns_array()
{
  NSArray* array = @[@"foo", @"bar"];
  BOOST_CHECK_EQUAL(elle::sprintf("%s", array), "(\n    foo,\n    bar\n)");
}

static
void
test_ns_dictionary()
{
  NSDictionary* dict = @{@"foo": @"bar"};
  BOOST_CHECK_EQUAL(elle::sprintf("%s", dict), "{\n    foo = bar;\n}");
}

static
void
test_ns_number()
{
  NSNumber* number = @42;
  BOOST_CHECK_EQUAL(elle::sprintf("%s", number), "42");
  NSNumber* number_uninitialized;
  BOOST_CHECK_EQUAL(elle::sprintf("%s", number_uninitialized), "<nil>");
}

static
void
test_ns_string()
{
  NSString* string = @" foo bar ";
  BOOST_CHECK_EQUAL(elle::sprintf("%s", string), " foo bar ");
  NSString* string_empty = @"";
  BOOST_CHECK_EQUAL(elle::sprintf("%s", string_empty), "<empty>");
  NSString* string_nil = nil;
  BOOST_CHECK_EQUAL(elle::sprintf("%s", string_nil), "<nil>");
  NSString* string_uninitialized;
  BOOST_CHECK_EQUAL(elle::sprintf("%s", string_uninitialized), "<nil>");
}

static
void
test_mix()
{
  NSString* s_foo = @"foo";
  NSString* s_bar = @"bar";
  NSString* s_uninitialized;
  NSNumber* number = @42;
  BOOST_CHECK_EQUAL(
    elle::sprintf("%s %s %s %s", s_foo, s_bar, s_uninitialized, number),
    "foo bar <nil> 42");
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(test_ns_array));
  suite.add(BOOST_TEST_CASE(test_ns_dictionary));
  suite.add(BOOST_TEST_CASE(test_ns_number));
  suite.add(BOOST_TEST_CASE(test_ns_string));
  suite.add(BOOST_TEST_CASE(test_mix));
}