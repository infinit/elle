#include "../cryptography.hh"

#include <cryptography/dh/KeyPair.hh>
#include <cryptography/dh/PublicKey.hh>
#include <cryptography/dh/PrivateKey.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/random.hh>

#include <elle/printf.hh>
#include <elle/types.hh>

/*---------.
| Generate |
`---------*/

static
infinit::cryptography::dh::KeyPair
_test_generate()
{
  infinit::cryptography::dh::KeyPair keypair =
    infinit::cryptography::dh::keypair::generate();

  return (keypair);
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
  infinit::cryptography::dh::KeyPair keypair1 =
    _test_generate();

  // KeyPair copy.
  infinit::cryptography::dh::KeyPair keypair2(keypair1);

  BOOST_CHECK_EQUAL(keypair1, keypair2);

  // KeyPair move.
  infinit::cryptography::dh::KeyPair keypair3(std::move(keypair1));

  BOOST_CHECK_EQUAL(keypair2, keypair3);

  // Attributes copy.
  infinit::cryptography::dh::KeyPair keypair4(keypair2.K(), keypair2.k());

  BOOST_CHECK_EQUAL(keypair2, keypair3);
  BOOST_CHECK_EQUAL(keypair2, keypair4);
  BOOST_CHECK_EQUAL(keypair3, keypair4);

  // Attributes move.
  infinit::cryptography::dh::PublicKey K(keypair3.K());
  infinit::cryptography::dh::PrivateKey k(keypair3.k());

  infinit::cryptography::dh::KeyPair keypair5(std::move(K), std::move(k));

  BOOST_CHECK_EQUAL(keypair2, keypair3);
  BOOST_CHECK_EQUAL(keypair2, keypair4);
  BOOST_CHECK_EQUAL(keypair2, keypair5);
  BOOST_CHECK_EQUAL(keypair3, keypair4);
  BOOST_CHECK_EQUAL(keypair3, keypair5);
  BOOST_CHECK_EQUAL(keypair4, keypair5);
}

/*--------.
| Operate |
`--------*/

static
void
_test_operate(infinit::cryptography::dh::KeyPair const& keypair1,
              infinit::cryptography::dh::KeyPair const& keypair2)
{
  // Agree on a shared secret key.
  infinit::cryptography::SecretKey secret1 =
    keypair1.k().agree(keypair2.K());
  infinit::cryptography::SecretKey secret2 =
    keypair2.k().agree(keypair1.K());

  BOOST_CHECK_EQUAL(secret1, secret2);
}

static
void
test_operate()
{
  infinit::cryptography::dh::KeyPair keypair1 =
    _test_generate();
  // XXX from keypair1's parameters
  infinit::cryptography::dh::KeyPair keypair2 =
    _test_generate();

  _test_operate(keypair1, keypair2);
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("dh/KeyPair");

  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));

  boost::unit_test::framework::master_test_suite().add(suite);
}
