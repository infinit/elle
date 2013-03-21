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

  // XXX
  {
    infinit::cryptography::KeyPair pair =
      infinit::cryptography::KeyPair::generate(
        infinit::cryptography::Cryptosystem::rsa, 1024);

    infinit::cryptography::Seed seed0 =
      infinit::cryptography::Seed::generate(pair);
    seed0.buffer().dump();
    /* XXX
    infinit::cryptography::Seed seed1 = pair.k().rotate(seed0);
    seed1.buffer().dump();

    infinit::cryptography::Seed seed2 = pair.k().rotate(seed1);
    seed2.buffer().dump();

    infinit::cryptography::KeyPair pair2(seed2);

    infinit::cryptography::Seed seed3 = pair.k().rotate(seed2);
    seed3.buffer().dump();

    infinit::cryptography::Seed _seed2 = pair.K().derive(seed3);
    _seed2.buffer().dump();

    infinit::cryptography::KeyPair _pair2(_seed2);

    BOOST_CHECK_EQUAL(pair2, _pair2);

    infinit::cryptography::Seed _seed1 = pair.K().derive(_seed2);
    _seed1.buffer().dump();

    infinit::cryptography::Seed _seed0 = pair.K().derive(_seed0);
    _seed0.buffer().dump();

    BOOST_CHECK_EQUAL(seed0, _seed0);
    */
  }
  // XXX

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
