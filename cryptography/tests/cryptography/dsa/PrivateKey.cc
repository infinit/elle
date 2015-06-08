#include "../cryptography.hh"

#include <cryptography/dsa/PrivateKey.hh>
#include <cryptography/dsa/PublicKey.hh>
#include <cryptography/dsa/KeyPair.hh>
#include <cryptography/Exception.hh>

#include <elle/printf.hh>
#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

/*----------.
| Represent |
`----------*/

static
void
_test_represent()
{
  infinit::cryptography::dsa::KeyPair keypair =
    infinit::cryptography::dsa::keypair::generate(1024);

  // 1)
  {
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << keypair.k();
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

  _test_represent();
}

/*---------.
| Generate |
`---------*/

static
infinit::cryptography::dsa::PrivateKey
_test_generate(elle::Natural32 const length = 1024)
{
  infinit::cryptography::dsa::KeyPair keypair =
    infinit::cryptography::dsa::keypair::generate(length);

  infinit::cryptography::dsa::PrivateKey k(keypair.k());

  return (k);
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
  infinit::cryptography::dsa::PrivateKey k1 = _test_generate(2048);

  // PrivateKey copy.
  infinit::cryptography::dsa::PrivateKey k2(k1);

  BOOST_CHECK_EQUAL(k1, k2);

  // PrivateKey move.
  infinit::cryptography::dsa::PrivateKey k3(std::move(k1));

  BOOST_CHECK_EQUAL(k2, k3);
}

/*--------.
| Compare |
`--------*/

static
void
test_compare()
{
  infinit::cryptography::dsa::PrivateKey k1 = _test_generate(1024);
  infinit::cryptography::dsa::PrivateKey k2 = _test_generate(1024);

  // With high probabilituy, this should not be the case. Otherwise,
  // the random generator is probably broken.
  BOOST_CHECK(k1 != k2);
  BOOST_CHECK(!(k1 == k2));
}

/*----------.
| Serialize |
`----------*/

static
void
test_serialize()
{
  // Serialize/deserialize.
  {
    infinit::cryptography::dsa::PrivateKey k1 = _test_generate(2048);

    elle::String archive;
    elle::serialize::to_string(archive) << k1;

    auto extractor = elle::serialize::from_string(archive);
    infinit::cryptography::dsa::PrivateKey k2(extractor);

    BOOST_CHECK_EQUAL(k1, k2);
  }

  // For every hard-coded strings [representation 1] in every format,
  // deserialize the public keys, re-serialize them, make sure they can
  // be used and finally upgrade them: useful for detecting changes in formats.
  {
    std::vector<elle::String> const archives{
      // format 0
      "AAAAAL4BAAAAAAAAMIIBugIBAAKBgQDF8rYSAqWqVtZVdKuFdptTmF9+KzUziuU3oGJjDDcyi7T4T08eYSULJPgWsYk23nvjkg4PzRZIAKt6gFRSrTmqXojjLqo+KZ2ysoP+Pia0UPmhGr0PNMyw1dEwojDTib1M3Y55S3K1zRtmhP2jLHh3JjwSbsVKWzxWo4mVHFx48QIVALjelZay31w+gs2F/zMhNTMk4D+jAoGAcbDW58U3jmviB26T7Rz5zRbujg+Pfui5hUL0QFA4pCfEzNtSain32zbKTM/c5JblvifxmwfhM+itbwUsyIKDkf5yGs6L6UsDiqgD+C36kCoYqk24qeFvzwJ0S+sLfGjtxckJ8yvG2/Dp6RwUOTg0Isfjk8kqKr3ov0WB/CkdJkkCgYAt7nVHMzGa8+r9iAId4fPR5+ibm7Rl4kvhuoQqOBplGM/QrsJpAtyAQ2Hk4pw+TuYvVSs5W4zxRLinBbucZaSjw8n9uG0VKN20RFDCgCEPGyEq/XtMgHXAWb61CzLPqQ01QtvJyMByh2QHdy8lGZeXfNRFSPi/VD4baw7s0nSjggIUWGE4vWLksXgl7QQ7rQ3IfV0BlgcEAA=="
    };

    infinit::cryptography::test::formats<
      infinit::cryptography::dsa::PrivateKey>(archives);
  }
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("dsa/PrivateyKey");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_compare));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}
