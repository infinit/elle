#include "cryptography.hh"

#include <cryptography/Seed.hh>
#include <cryptography/KeyPair.hh>
#include <cryptography/Exception.hh>
#include <cryptography/random.hh>

#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

/*--------.
| Helpers |
`--------*/

static
infinit::cryptography::KeyPair
_test_generate_rsa(elle::Natural32 const length)
{
  infinit::cryptography::KeyPair pair =
    infinit::cryptography::KeyPair::generate(
      infinit::cryptography::Cryptosystem::rsa,
      length);

  return (pair);
}

/*----------.
| Represent |
`----------*/

static
void
test_represent_rsa()
{
  // 1)
  {
    infinit::cryptography::KeyPair pair = _test_generate_rsa(1024);
    infinit::cryptography::Seed seed =
      infinit::cryptography::Seed::generate(pair);
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << pair;
    elle::printf("[representation 1] %s\n", archive);
  }
}

static
void
test_represent()
{
  // WARNING: To uncomment only if one wants to update the representations.
  return;

  // These generate base64-based representations which can be used in
  // other tests.

  // RSA.
  test_represent_rsa();
}

/*---------.
| Generate |
`---------*/

static
infinit::cryptography::Seed
test_generate_rsa()
{
  infinit::cryptography::KeyPair pair = _test_generate_rsa(1024);

  infinit::cryptography::Seed seed =
    infinit::cryptography::Seed::generate(pair);

  return (seed);
}

static
void
test_generate()
{
  // RSA.
  test_generate_rsa();
}

/*----------.
| Construct |
`----------*/

static
void
test_construct()
{
  // RSA
  infinit::cryptography::Seed seed1 = test_generate_rsa();

  // Seed copy.
  infinit::cryptography::Seed seed2(seed1);

  BOOST_CHECK_EQUAL(seed1, seed2);

  // Seed move.
  infinit::cryptography::Seed seed3(std::move(seed1));

  BOOST_CHECK_EQUAL(seed2, seed3);
}

/*--------.
| Operate |
`--------*/

static
void
test_operate()
{
  // RSA.
  infinit::cryptography::KeyPair pair = _test_generate_rsa(1024);

  // Perform a sequence of rotation/derivation.
  {
    infinit::cryptography::Seed seed0 =
      infinit::cryptography::Seed::generate(pair);

    infinit::cryptography::Seed seed1 = pair.k().rotate(seed0);

    // XXX
    elle::printf("seed1: %s\n", seed1);

    infinit::cryptography::KeyPair pair1(seed1);
    infinit::cryptography::PublicKey K1(seed1);
    infinit::cryptography::PrivateKey k1(seed1);

    infinit::cryptography::Seed seed2 = pair.k().rotate(seed1);

    // XXX
    elle::printf("seed2: %s\n", seed2);

    infinit::cryptography::KeyPair pair2(seed2);
    infinit::cryptography::PublicKey K2(seed2);
    infinit::cryptography::PrivateKey k2(seed2);

    infinit::cryptography::Seed seed3 = pair.k().rotate(seed2);

    // XXX
    elle::printf("seed3: %s\n", seed3);

    infinit::cryptography::Seed _seed2 = pair.K().derive(seed3);

    // XXX
    elle::printf("_seed2: %s\n", _seed2);

    infinit::cryptography::KeyPair _pair2(_seed2);
    infinit::cryptography::PublicKey _K2(_seed2);
    infinit::cryptography::PrivateKey _k2(_seed2);

    infinit::cryptography::Seed _seed1 = pair.K().derive(_seed2);

    // XXX
    elle::printf("_seed1: %s\n", _seed1);

    infinit::cryptography::KeyPair _pair1(_seed1);
    infinit::cryptography::PublicKey _K1(_seed1);
    infinit::cryptography::PrivateKey _k1(_seed1);

    infinit::cryptography::Seed _seed0 = pair.K().derive(_seed0);

    // XXX
    elle::printf("_seed0: %s\n", _seed0);

    BOOST_CHECK_EQUAL(seed2, _seed2);
    BOOST_CHECK_EQUAL(pair2, _pair2);
    BOOST_CHECK_EQUAL(_pair2.K(), _K2);
    BOOST_CHECK_EQUAL(_pair2.k(), _k2);

    BOOST_CHECK_EQUAL(seed1, _seed1);
    BOOST_CHECK_EQUAL(pair1, _pair1);
    BOOST_CHECK_EQUAL(_pair1.K(), _K1);
    BOOST_CHECK_EQUAL(_pair1.k(), _k1);

    BOOST_CHECK_EQUAL(seed0, _seed0);

    // XXX check que la taille est la meme entre toutes les seed (castees en RSA)
  }

  // XXX deirvation with wrong seed size -> throw
}

/*----------.
| Serialize |
`----------*/

static
void
test_serialize()
{
  // XXX + represent()
}

/*-----.
| Main |
`-----*/

static
bool
test()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("Seed");

  suite->add(BOOST_TEST_CASE(test_represent));
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
