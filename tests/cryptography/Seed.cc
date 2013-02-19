#include "cryptography.hh"
#include "Sample.hh"

#include <cryptography/Seed.hh>
#include <cryptography/KeyPair.hh>
#include <cryptography/Exception.hh>

#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

/*----------.
| Represent |
`----------*/

void
test_represent()
{
  // These generate base64-based representations which can be used in
  // other tests.

  // XXX
}

/*---------.
| Generate |
`---------*/

void
test_generate()
{
  // XXX
}

/*----------.
| Construct |
`----------*/

void
test_construct()
{
  // XXX
}

/*--------.
| Operate |
`--------*/

void
test_operate()
{
  // XXX
}

/*----------.
| Serialize |
`----------*/

void
test_serialize()
{
  // XXX
}

/*-----.
| Main |
`-----*/

bool
test()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("Seed");

  // To uncomment if one wants to update the representations.
  //suite->add(BOOST_TEST_CASE(test_represent));

  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);

  return (true);
}

int
main(int argc,
     char** argv)
{
  return (boost::unit_test::unit_test_main(test, argc, argv));
}
