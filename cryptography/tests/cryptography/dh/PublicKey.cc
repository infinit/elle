#include "../cryptography.hh"

#include <cryptography/dh/PublicKey.hh>
#include <cryptography/dh/PrivateKey.hh>
#include <cryptography/dh/KeyPair.hh>

/*---------.
| Generate |
`---------*/

static
infinit::cryptography::dh::PublicKey
_test_generate()
{
  infinit::cryptography::dh::KeyPair keypair =
    infinit::cryptography::dh::keypair::generate();

  infinit::cryptography::dh::PublicKey K(keypair.K());

  return (K);
}

static
void
test_generate()
{
  _test_generate();
}

/*----------.
| Construct |
`----------*/

static
void
test_construct()
{
  infinit::cryptography::dh::PublicKey K1 = _test_generate();

  // PublicKey copy.
  infinit::cryptography::dh::PublicKey K2(K1);

  BOOST_CHECK_EQUAL(K1, K2);

  // PublicKey move.
  infinit::cryptography::dh::PublicKey K3(std::move(K1));

  BOOST_CHECK_EQUAL(K2, K3);
}

/*--------.
| Compare |
`--------*/

static
void
test_compare()
{
  infinit::cryptography::dh::PublicKey K1 = _test_generate();
  infinit::cryptography::dh::PublicKey K2 = _test_generate();

  // With high probability, this should not be the case. Otherwise,
  // the random generator is probably broken.
  BOOST_CHECK(K1 != K2);
  BOOST_CHECK(!(K1 == K2));
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("dh/PublicyKey");

  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_compare));

  boost::unit_test::framework::master_test_suite().add(suite);
}
