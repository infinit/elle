#include "../cryptography.hh"

#include <elle/cryptography/dh/KeyPair.hh>
#include <elle/cryptography/dh/PublicKey.hh>
#include <elle/cryptography/dh/PrivateKey.hh>
#include <elle/cryptography/Oneway.hh>
#include <elle/cryptography/Cipher.hh>
#include <elle/cryptography/random.hh>

#include <elle/printf.hh>

/*---------.
| Generate |
`---------*/

static
elle::cryptography::dh::KeyPair
_test_generate()
{
  elle::cryptography::dh::KeyPair keypair =
    elle::cryptography::dh::keypair::generate();

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
  elle::cryptography::dh::KeyPair keypair1 =
    _test_generate();

  // KeyPair copy.
  elle::cryptography::dh::KeyPair keypair2(keypair1);

  BOOST_CHECK_EQUAL(keypair1, keypair2);

  // KeyPair move.
  elle::cryptography::dh::KeyPair keypair3(std::move(keypair1));

  BOOST_CHECK_EQUAL(keypair2, keypair3);

  // Attributes copy.
  elle::cryptography::dh::KeyPair keypair4(keypair2.K(), keypair2.k());

  BOOST_CHECK_EQUAL(keypair2, keypair3);
  BOOST_CHECK_EQUAL(keypair2, keypair4);
  BOOST_CHECK_EQUAL(keypair3, keypair4);

  // Attributes move.
  elle::cryptography::dh::PublicKey K(keypair3.K());
  elle::cryptography::dh::PrivateKey k(keypair3.k());

  elle::cryptography::dh::KeyPair keypair5(std::move(K), std::move(k));

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
_test_operate(elle::cryptography::dh::KeyPair const& keypair1,
              elle::cryptography::dh::KeyPair const& keypair2)
{
  // Agree on a shared secret key.
  elle::cryptography::SecretKey secret1 =
    keypair1.k().agree(keypair2.K());
  elle::cryptography::SecretKey secret2 =
    keypair2.k().agree(keypair1.K());

  BOOST_CHECK_EQUAL(secret1, secret2);
}

static
void
test_operate()
{
  elle::cryptography::dh::KeyPair keypair1 =
    _test_generate();
  // XXX from keypair1's parameters
  elle::cryptography::dh::KeyPair keypair2 =
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
