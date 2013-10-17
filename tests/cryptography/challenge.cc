#include "cryptography.hh"

#include <cryptography/challenge.hh>
#include <cryptography/KeyPair.hh>

/*--------.
| Operate |
`--------*/

static
void
test_operate()
{
  infinit::cryptography::KeyPair challenger =
    infinit::cryptography::KeyPair::generate(
      infinit::cryptography::Cryptosystem::rsa,
      2048);
  infinit::cryptography::KeyPair challengee =
    infinit::cryptography::KeyPair::generate(
      infinit::cryptography::Cryptosystem::rsa,
      1024);

  elle::String nonce = infinit::cryptography::challenge::nonce();

  elle::String challenge =
    infinit::cryptography::challenge::create(challenger.K(),
                                             challengee.K(),
                                             nonce);

  elle::String response =
    infinit::cryptography::challenge::accept(challenge,
                                             challengee,
                                             challenger.K());

  BOOST_CHECK_EQUAL(infinit::cryptography::challenge::check(response,
                                                            challenger.k(),
                                                            nonce),
                    true);
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("challenge");

  suite->add(BOOST_TEST_CASE(test_operate));

  boost::unit_test::framework::master_test_suite().add(suite);
}

