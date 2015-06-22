#include "../cryptography.hh"
#include "../Sample.hh"

#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/KeyPair.hh>
#include <cryptography/Exception.hh>

#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

/*----------.
| Represent |
`----------*/

static elle::String const _input1("my balls are ok!");
static Sample const _input2(23293083121, "chodaboy");

static
void
_test_represent()
{
  infinit::cryptography::rsa::KeyPair keypair =
    infinit::cryptography::rsa::keypair::generate(2048);

  // 1)
  {
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << keypair.K();
    elle::printf("[representation 1] %s\n", archive);
  }

  // 2)
  {
    infinit::cryptography::Signature signature =
      keypair.k().sign(
        infinit::cryptography::Plain{_input1});
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << signature;
    elle::printf("[representation 2] %s\n", archive);
  }

  // 3)
  {
    infinit::cryptography::Signature signature = keypair.k().sign(_input2);
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << signature;
    elle::printf("[representation 3] %s\n", archive);
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
infinit::cryptography::rsa::PublicKey
_test_generate(elle::Natural32 const length = 1024)
{
  infinit::cryptography::rsa::KeyPair keypair =
    infinit::cryptography::rsa::keypair::generate(length);

  infinit::cryptography::rsa::PublicKey K(keypair.K());

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
  infinit::cryptography::rsa::PublicKey K1 = _test_generate(2048);

  // PublicKey copy.
  infinit::cryptography::rsa::PublicKey K2(K1);

  BOOST_CHECK_EQUAL(K1, K2);

  // PublicKey move.
  infinit::cryptography::rsa::PublicKey K3(std::move(K1));

  BOOST_CHECK_EQUAL(K2, K3);
}

/*--------.
| Operate |
`--------*/

static
void
test_operate()
{
  // Construct a public key from [representation 1]
  elle::String representation("AgAAAA4BAAAAAAAAMIIBCgKCAQEAsqtJHBydi4nQZVQFQqb1IawQiA4d/JWXXWZihq2PSroyC4dzDo8V410W9gQ9iQRyBeUdPWDXWxJeMwTZ94kcBMn2De0vtViDYr1O+0UT1Ka+pAMxXV4csDDD8v6h8QqnNXkf734fEcNp6a1O+QtzypnxJnLmoC7VbLFmrJ5uCNMH2spgqfaSF9Y9/5CFGiWuq0DWIbteTjUWAfDqGQsDEJiW6WiwqOqkKMswA0uU9psznu6yoH1OLkVbH/FYGNqfzow1FnbuAunSfC2d2AIbljrPpWaanD6xQ6MzNFRUOcg6QQ5QDa+l1z79BUv8eu4eMgBpz5gm3/KWxnPA3E9SDwIDAQABAwAFAAQACwABAA==");

  auto extractor =
    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(representation);
  infinit::cryptography::rsa::PublicKey K(extractor);

  // The following operations are based on hard-coded base64 string which
  // represent the data on which to operate, in their serialized form.

  // Verify plain from [representation 2].
  {
    elle::String archive("AAAAAAABAAAAAAAAb+eObSV2/U08nAwsBNQE7/XOSrq6r/IH36g/3RecZy5m3xm02h6U1KwWfA31cRrz+IKD5PdMeqlnz+mZvJvugnIq5/LHLIquII+bDEntxGpRZfIddL14ijYtvO57tNnrejW+N9K7rh5a2DpVVoc4qC/cG9KlLF/zqg4BeMPf9oA1VIHXbQhNjR0t1W3rr2GpOII5gxgRc39KhngJ+FysEv2Qb3q7q59BcwkoXcjGNXSw8qRuJPn2deUEUMqvJsMfNS5Mew954e4Z7NJY3Glo7038lVi2LHNIQDTaCHUlQ3tGJ8GcRHPPl4jY8JKF4iK/quU+VPewcuzIB2p/yfVChw==");

    auto extractor =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive);
    infinit::cryptography::Signature signature(extractor);

    auto result =
      K.verify(signature,
               infinit::cryptography::Plain{_input1});

    BOOST_CHECK_EQUAL(result, true);
  }

  // Verify plain from [representation 3].
  {
    elle::String archive("AAAAAAABAAAAAAAAZW0IAT7dLizK21ZGQoZZNv/UpDmyxMEE9X/Sy/VmfFO/oexDr8Y1b9tO6FPxJywQsE3S6OWWbiLIZZ0xR0g1q9jrklYZ1UjAHNNZoErskD7LsDP5fFupCMDlwoPoegX2mK7u95fxYdZo2Rz2N/uJ6gxEDMKkypmVlbUZRB5ozHVHVUadQ/j0JjgtXMfXbrZrsi1vP24IplxWDcGNOHfIWcd0eerUEWNuwvMW5WXzQtq4H1nmSaFQ8NRMkU80EYm88BJkFAnn/Q1ailQ4eeiskiaAic+Ti7Ab6ztr5K3V+5wY7H24OX4u3B83Dch6hrIfhGFx9J1n6AUTjtTmcew0dQ==");

    auto extractor =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive);
    infinit::cryptography::Signature signature(extractor);

    auto result = K.verify(signature, _input2);

    BOOST_CHECK_EQUAL(result, true);
  }
}

/*--------.
| Compare |
`--------*/

static
void
test_compare()
{
  infinit::cryptography::rsa::PublicKey K1 = _test_generate(1024);
  infinit::cryptography::rsa::PublicKey K2 = _test_generate(1024);

  // With high probability, this should not be the case. Otherwise,
  // the random generator is probably broken.
  BOOST_CHECK(K1 != K2);
  BOOST_CHECK(!(K1 == K2));
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
    infinit::cryptography::rsa::PublicKey K1 = _test_generate(2048);

    elle::String archive;
    elle::serialize::to_string(archive) << K1;

    auto extractor = elle::serialize::from_string(archive);
    infinit::cryptography::rsa::PublicKey K2(extractor);

    BOOST_CHECK_EQUAL(K1, K2);
  }

  // For every hard-coded strings [representation 1] in every format,
  // deserialize the public keys, re-serialize them, make sure they can
  // be used and finally upgrade them: useful for detecting changes in formats.
  {
    std::vector<elle::String> const archives{
      // format 0
      "AAAAAAAAAACAAAAApcZBa1TEkWy17WfZVvNGHDLufkKpu3HRBZ/r4pPtdCKufOBKYw22eWxKEClEo06IfnuCtzfn/ZfSGDMelzJKy5XhJ+qX1e7IYTioWcUq928awLyK/4mAShE3lv5BQPDx983+1LEigaYmRcD1Dii1sSMyezvxCVyufYWD2NOQjMMAAAMAAAABAAE=",
      // format 1
      "AAAAAAEAAAAAAQAAzJcO4BIlHIdoQUYGiMvjSG+U4434tyZOhRnfRJUxd9tK/zxezIwPQOBShjU5zin4rhXholbVKJxOLrMN29AoF+wJi3c8+GCFQFBH5sJAQwf66M6kIOdtY2rtIIUd8H4SxWNe9j7MpYQxaYWpMlJE578oZ2/X4aYOO/5Ki4jA4X+afRjaN3yJnikG7OkkwndRLzQzR2BTmG6zT5hGrOZQ13G1SmeMxI5REK6rjrAsrbNPi9yojrjGK5UyityKy2yjiJSf7br7f34sG1xIhKcUW1MWUxi0gaFR4X/6TbTeIxXuuRIgyY3OLWZx3O8Vlx7jwcWcPG/kY5RPbdfU29x9lwAAAwAAAAEAAQ==",
      // format 2
      "AgAAAA4BAAAAAAAAMIIBCgKCAQEAsqtJHBydi4nQZVQFQqb1IawQiA4d/JWXXWZihq2PSroyC4dzDo8V410W9gQ9iQRyBeUdPWDXWxJeMwTZ94kcBMn2De0vtViDYr1O+0UT1Ka+pAMxXV4csDDD8v6h8QqnNXkf734fEcNp6a1O+QtzypnxJnLmoC7VbLFmrJ5uCNMH2spgqfaSF9Y9/5CFGiWuq0DWIbteTjUWAfDqGQsDEJiW6WiwqOqkKMswA0uU9psznu6yoH1OLkVbH/FYGNqfzow1FnbuAunSfC2d2AIbljrPpWaanD6xQ6MzNFRUOcg6QQ5QDa+l1z79BUv8eu4eMgBpz5gm3/KWxnPA3E9SDwIDAQABAwAFAAQACwABAA=="
      };

    infinit::cryptography::test::formats<
      infinit::cryptography::rsa::PublicKey>(archives);
  }
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("rsa/PublicyKey");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_compare));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}
