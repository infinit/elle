#include <algorithm>
#include <list>
#include <sstream>
#include <string>
#include <vector>

#include <elle/container/list.hh>
#include <elle/format/json/Array.hh>
#include <elle/serialize/Concrete.hh>
#include <elle/serialize/JSONArchive.hh>
#include <elle/serialize/ListSerializer.hxx>
#include <elle/serialize/extract.hh>
#include <elle/serialize/insert.hh>
#include <elle/test.hh>

using namespace elle::serialize;
// XXX: What about pointers?

// - Basic case ----------------------------------------------------------------
// XXX: float and double with a null fractional part are not serialized well.
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
            this->d == a.d
      );
  }
};

static
std::ostream&
operator <<(std::ostream& out,
            A const& a)
{
  out << "{"
      << a.i << ", "
      << a.str << ", "
      << a.c << ", "
      << a.f << ", "
      << a.d << "}";
  return out;
}

ELLE_SERIALIZE_SIMPLE(A, ar, value,version)
{
  (void) version;
  ar & named("i", value.i);
  ar & named("str", value.str);
  ar & named("c", value.c);
  ar & named("f", value.f);
  ar & named("d", value.d);
}

static
void
SimpleClassSerialization()
{
  // XXX: If d has a null fractional part, deserialization failed.
  // XXX: Also failed with double d = 6.7f;
  double d = 6.7;
  A from{1, "deux", '3', 4.5f, d};
  A to;
  std::string serialized{};
  std::string reserialized{};

  to_string<OutputJSONArchive>(serialized) << from;
  from_string<InputJSONArchive>(serialized) >> to;
  to_string<OutputJSONArchive>(reserialized) << to;

  BOOST_CHECK_EQUAL(serialized, reserialized);
  BOOST_CHECK_EQUAL(from, to);
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

  E(D d = D{}):
    d(d)
  {}

  bool
  operator ==(E const& e) const
  {
    return (this->d == e.d);
  }
};

static
std::ostream&
operator <<(std::ostream& out,
            D const& d)
{
  return out << d.i;
}

static
std::ostream&
operator <<(std::ostream& out,
            E const& e)
{
  return out << e.d;
}


ELLE_SERIALIZE_SIMPLE(D, ar, value,version)
{
  (void) version;
  ar & named("i", value.i);
}

ELLE_SERIALIZE_SIMPLE(E, ar, value,version)
{
  (void) version;
  ar & named("d", value.d);
}

static
void
AggregatedClassSerialization()
{
  E from{1};
  E to;
  std::string serialized{};
  std::string reserialized{};

  to_string<OutputJSONArchive>(serialized) << from;
  from_string<InputJSONArchive>(serialized) >> to;

  BOOST_CHECK_EQUAL(from, to);

  to_string<OutputJSONArchive>(reserialized) << to;

  BOOST_CHECK_EQUAL(serialized, reserialized);
}

// - Inheritance ---------------------------------------------------------------
struct B
{
  double i;

  B(double i = 0):
    i(i)
  {}
  bool
  operator ==(B const& c) const
  {
    return this->i == c.i;
  }
};

struct C:
  public B
{
  float j;
  std::string k;
  B l;

  explicit
  C(double i = 0,
    float j = 0):
    B(i),
    j(j),
    k{"salut"},
    l{j * 2.0f}
  {}

  bool
  operator ==(C const& c) const
  {
    return (this->i == c.i &&
            this->j == c.j &&
            this->k == c.k &&
            this->l == c.l);
  }
};

static
std::ostream&
operator <<(std::ostream& out,
            C const& c)
{
  return out << "{"
             << c.i << ", " << c.j << ", " << c.k << ", " << c.l.i
             << "}";
}

ELLE_SERIALIZE_SIMPLE(B, ar, value,version)
{
  (void) version;
  ar & named("i", value.i);
}

ELLE_SERIALIZE_SIMPLE(C, ar, value,version)
{
  (void) version;
  ar & base_class<B>(value);
  ar & named("j", value.j);
  ar & named("l", value.l);
  ar & named("k", value.k);
}

static
void
InheritedClassSerialization()
{
  C from{1.0, 1.42f};
  C to;
  std::string serialized{};

  to_string<OutputJSONArchive>(serialized) << from;
  from_string<InputJSONArchive>(serialized) >> to;

  BOOST_CHECK_EQUAL(from, to);

  {
    std::string reserialized{};
    to_string<OutputJSONArchive>(reserialized) << from;
    BOOST_CHECK_EQUAL(serialized, reserialized);
  }
  {
    std::string reserialized{};
    to_string<OutputJSONArchive>(reserialized) << to;
    BOOST_CHECK_EQUAL(serialized, reserialized);
  }
}

// - Containers ----------------------------------------------------------------

struct Obj
{
  int i;

  bool
  operator <(Obj const& obj) const
  {
    return (this->i < obj.i);
  }

  bool
  operator ==(Obj const& obj) const
  {
    return (this->i == obj.i);
  }
};

static
std::ostream&
operator <<(std::ostream& out,
            Obj const& obj)
{
  return out << obj.i;
}

ELLE_SERIALIZE_SIMPLE(Obj, ar, value,version)
{
  (void) version;
  ar & named("i", value.i);
}

// XXX: map, multimap, set, ...
struct SContainers
{
  std::list<std::string> ls;
  std::list<int> li;
  std::list<Obj> lo;
  std::vector<int> vi;
  std::vector<std::string> vs;
  std::vector<Obj> vo;

  bool
  operator ==(SContainers const& obj) const
  {
    return (std::equal(this->ls.cbegin(), this->ls.cend(), obj.ls.cbegin()) &&
            std::equal(this->li.cbegin(), this->li.cend(), obj.li.cbegin()) &&
            std::equal(this->lo.cbegin(), this->lo.cend(), obj.lo.cbegin()) &&
            std::equal(this->vi.cbegin(), this->vi.cend(), obj.vi.cbegin()) &&
            std::equal(this->vo.cbegin(), this->vo.cend(), obj.vo.cbegin()) &&
            std::equal(this->vs.cbegin(), this->vs.cend(), obj.vs.cbegin())
      );
  }
};

static
std::ostream&
operator <<(std::ostream& out,
            SContainers const& obj)
{
  return out
    << obj.ls << std::endl
    << obj.li << std::endl
    << obj.lo << std::endl
    << obj.vi << std::endl
    << obj.vs << std::endl
    << obj.vo
    ;
}

ELLE_SERIALIZE_SIMPLE(SContainers, ar, value,version)
{
  (void) version;
  ar & named("ls", value.ls);
  ar & named("li", value.li);
  ar & named("lo", value.lo);
  ar & named("vs", value.vs);
  ar & named("vi", value.vi);
  ar & named("vo", value.vo);
}

static
void
ContainersSerialization()
{
  SContainers from{
    {"1", "2"},
    {5, 6},
    {{9}, {10}},
    {3, 4},
    {"7", "8"},
    {{11}, {12}}
  };

  SContainers to;
  std::string serialized{};
  std::string reserialized{};

  to_string<OutputJSONArchive>(serialized) << from;
  elle::print("SERIALIZED:", serialized);
  from_string<InputJSONArchive>(serialized) >> to;
  to_string<OutputJSONArchive>(reserialized) << to;

  BOOST_CHECK_EQUAL(serialized, reserialized);
  BOOST_CHECK_EQUAL(from, to);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();

  suite.add(BOOST_TEST_CASE(SimpleClassSerialization));
  suite.add(BOOST_TEST_CASE(AggregatedClassSerialization));
  suite.add(BOOST_TEST_CASE(InheritedClassSerialization));
  suite.add(BOOST_TEST_CASE(ContainersSerialization));
}
