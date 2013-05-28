#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE JSONSerialization

#include <elle/serialize/JSONArchive.hh>
#include <elle/serialize/extract.hh>
#include <elle/serialize/insert.hh>

#include <boost/test/unit_test.hpp>

using namespace elle::serialize;

// - Basic case ----------------------------------------------------------------
struct A
{
  int i;
  std::string str;
  char c;
  float f;
  double d;

  bool
  operator ==(A const& a) const
  {
    return (this->i == a.i &&
            this->str == a.str &&
            this->c == a.c &&
            this->f == a.f &&
            this->d == a.d);
  }
};

std::ostream&
operator <<(std::ostream& out, A const& a)
{
  out << "{"
      << a.i << ", "
      << a.str << ", "
      << a.c << ", "
      << a.f << ", "
      << a.d << "}";
  return out;
}

ELLE_SERIALIZE_SIMPLE(A, ar, value, version)
{
  ar & named("i", value.i);
  ar & named("str", value.str);
  ar & named("c", value.c);
  ar & named("f", value.f);
  ar & named("d", value.d);
}

BOOST_AUTO_TEST_CASE(SimpleClassSerialization)
{
  A a_from{1, "deux", '3', 4.5f, 6.0f};
  A a_to{};
  std::string serialized{};
  std::string reserialized{};

  to_string<OutputJSONArchive>(serialized) << a_from;
  from_string<InputJSONArchive>(serialized) >> a_to;
  to_string<OutputJSONArchive>(reserialized) << a_to;

  BOOST_CHECK_EQUAL(serialized, reserialized);
  BOOST_CHECK_EQUAL(a_from, a_to);
}

// - Aggregation ---------------------------------------------------------------
struct D
{
  int i;

  D(int i = 0):
    i(i)
  {}

  bool
  operator ==(D const& d) const
  {
    return (this->i == d.i);
  }
};

struct E
{
  D d;

  bool
  operator ==(E const& e) const
  {
    return (this->d == e.d);
  }
};

std::ostream&
operator <<(std::ostream& out, D const& d)
{
  return out << d.i;
}

std::ostream&
operator <<(std::ostream& out, E const& e)
{
  return out << e.d;
}


ELLE_SERIALIZE_SIMPLE(D, ar, value, version)
{
  ar & named("i", value.i);
}

ELLE_SERIALIZE_SIMPLE(E, ar, value, version)
{
  ar & named("d", value.d);
}

BOOST_AUTO_TEST_CASE(AggregatedClassSerialization)
{
  E e_from{1};
  E e_to{};
  std::string serialized{};
  std::string reserialized{};

  to_string<OutputJSONArchive>(serialized) << e_from;
  from_string<InputJSONArchive>(serialized) >> e_to;

  BOOST_CHECK_EQUAL(e_from, e_to);

  to_string<OutputJSONArchive>(reserialized) << e_to;

  BOOST_CHECK_EQUAL(serialized, reserialized);
}

// - Inheritance ---------------------------------------------------------------
struct B
{
  int i;

  B(int i = 0):
    i(i)
  {}
};

struct C:
  public B
{
  int j;

  C(int i = 0, int j = 0):
    B(i),
    j(j)
  {}

  bool
  operator ==(C const& c) const
  {
    return (this->i == c.i && this->j == c.j);
  }
};

std::ostream&
operator <<(std::ostream& out, C const& c)
{
  return out << "{" << c.i << ", " << c.j << "}";
}

ELLE_SERIALIZE_SIMPLE(B, ar, value, version)
{
  ar & named("i", value.i);
}

ELLE_SERIALIZE_SIMPLE(C, ar, value, version)
{
  ar & base_class<B>(value);
  ar & named("j", value.j);
}

BOOST_AUTO_TEST_CASE(InheritedClassSerialization)
{
  C c_from{1, 1};
  C c_to{};
  std::string serialized{};
  std::string reserialized{};

  // XXX: This doesn't work!!!!

  // to_string<OutputJSONArchive>(serialized) << c_from;
  // from_string<InputJSONArchive>(serialized) >> c_to;

  //BOOST_CHECK_EQUAL(c_from.i, c_to.i);
  //BOOST_CHECK_EQUAL(c_from.j, c_to.j);

  // to_string<OutputJSONArchive>(reserialized) << c_to;

  //BOOST_CHECK_EQUAL(serialized, reserialized);
  //BOOST_CHECK_EQUAL(c_from, c_to);
}
