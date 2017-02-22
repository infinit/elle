#include <elle/Exception.hh>
#include <elle/concept/Uniquable.hh>
#include <elle/serialize/BinaryArchive.hh>
#include <elle/serialize/extract.hh>
#include <elle/serialize/insert.hh>
#include <elle/serialize/construct.hh>
#include <elle/test.hh>

using elle::serialize::to_string;
using elle::serialize::from_string;
using elle::serialize::InputBase64Archive;
using elle::serialize::OutputBase64Archive;

struct A
{
  std::string data;
  A(std::string const& data = ""):
    data(data)
  {}
  ELLE_SERIALIZE_CONSTRUCT(A) {};
  ELLE_SERIALIZE_FRIEND_FOR(A);
};


ELLE_SERIALIZE_SIMPLE(A, archive, value, format)
{
  (void)format;
  archive & value.data;
}

static
void
extract()
{
  // Generated this way
  std::string uniq;
  A a {"bite"};

  to_string<OutputBase64Archive>(uniq) << a;

  A b{from_string<InputBase64Archive>(uniq)};

  BOOST_CHECK_EQUAL(a.data, b.data);
}


static
void
extract_error()
{
  // Generated this way
  std::string uniq = "toto==";

  A a;
  BOOST_CHECK_THROW(a = A{from_string<InputBase64Archive>(uniq)},
                    elle::Exception);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();

  suite.add(BOOST_TEST_CASE(extract));
  suite.add(BOOST_TEST_CASE(extract_error));
}
