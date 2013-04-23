#define BOOST_TEST_MODULE footprint
#define BOOST_TEST_DYN_LINK

#include <elle/serialize/footprint.hh>
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>

struct A
{
  std::string a;
  std::string b;
  std::string c;
  float       float_;
  int         int_;
  short       short_;

  bool operator ==(A const& o) const
  {
    return a == o.a &&
           b == o.b &&
           c == o.c &&
           float_ == o.float_ &&
           int_ == o.int_ &&
           short_ == o.short_;
  }
};

ELLE_SERIALIZE_SIMPLE(A, archive, value, version)
{
  (void)version;
  archive & named("a", value.a);
  archive & named("b", value.b);
  archive & named("c", value.c);
  archive & named("a float", value.float_);
  archive & named("an int", value.int_);
  archive & named("a short", value.short_);
}

BOOST_AUTO_TEST_CASE(footprint_diff)
{
  A a {"un", "deux", "trois", float(0.1f), int(1), short(64)};
  A b {"trois", "deux", "un", float(0.2f), int(2), short(65)};

  int size_a = elle::serialize::footprint(a);
  int size_b = elle::serialize::footprint(b);

   BOOST_CHECK_EQUAL(size_a, size_b);
}

BOOST_AUTO_TEST_CASE(footprint_same)
{
  A a {"un", "deux", "trois", float(0.1f), int(1), short(64)};
  A b {"un", "deux", "trois", float(0.1f), int(1), short(64)};

  int size_a = elle::serialize::footprint(a);
  int size_b = elle::serialize::footprint(b);

   BOOST_CHECK_EQUAL(size_a, size_b);
}

BOOST_AUTO_TEST_CASE(footprint_notsame)
{
  A a {"un", "deux", "trois", float(0.1f), int(1), short(64)};
  A b {"foo", "bar", "foobarbarfoo", float(0), int(1), short(64)};

  int size_a = elle::serialize::footprint(a);
  int size_b = elle::serialize::footprint(b);

   BOOST_CHECK_NE(size_a, size_b);
}

BOOST_AUTO_TEST_CASE(footprint)
{
  A a;

  elle::Natural32 size = elle::serialize::footprint(a);

  BOOST_CHECK_EQUAL(size, 24);
}
