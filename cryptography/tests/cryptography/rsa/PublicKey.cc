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
  elle::String representation("AAAAAA4BAAAAAAAAMIIBCgKCAQEA7pNuopCQXfvUCCjJFKXgACIKu3bwGM77rCreRpv2+VW7mcv1tij1Q4Gsl5ArBfSSmMTWHhmLMhPJCR6MF8QGVlB10w49ED2OxNxoSz35VrUIkmbpCNqGXsySS/d4V0rjPQZUugPngBELJzvrNnwfinuWfK5cwZBLBEz0y/Ugcx1qdyXxvqmRfWhF/dP2IgGxm52gnRmSb78Mydzcgnaxf4808a+X6cp45VuEDFsRli4EIRte144V+fEFjt/gRyjUWXlqfYzPsBLaSaGfEqL8Oiy322/cNNaxsS74+n9tmMPOLGDtzEF01lGVBplKZx9OX6lD4JjnCyNOTuK2TWAbcQIDAQABAwAFAAQACwABAA==");

  auto extractor =
    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(representation);
  infinit::cryptography::rsa::PublicKey K(extractor);

  // The following operations are based on hard-coded base64 string which
  // represent the data on which to operate, in their serialized form.

  // Verify plain from [representation 2].
  {
    elle::String archive("AAAAAAABAAAAAAAAHckJcfuSWQeSsKOmq5sHktw0Ann2mcnpUvjghUHVK/Nj3rc9I5tyetRJts6jBx9nY7yIvuX0doNHIu39uDvISnR9p6598ncAxZm9Q9F9ClWYilrlnLfA76y+gFtOT4C8fmzbjnaGAFa3cRG7ypk8K2NEXdKCYb3MwU1XrqYM9QmNlEPFfbYretJb2V914JcrzHvJeb/xlGbNF+3B1914qJtRJ/fHhBM0AB6jtkuyMYqNZD5NcnFDWvQElDHHjUEwHzMqN108A+8R98lO+ZOvRoz/n9x1/BI4OUYBLynoK5wVjrFKM9B2uKs6rpBNLCNz2m3B+p+WY9qtYQwjh1kVOA==");

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
    elle::String archive("AAAAAAABAAAAAAAAt3jeL8l6tceatgjmwwvnpppobTNql1SZY5scB2UIqJZzuEF7V7jNYYrCGBeppjWZrYQOEI270faJBLDsRy8QlYq3ZlGOCVbWVJfLlaYRqHGv1+YkH7RHDtJ9FECdRrk7Pjhd88yrwAghruNbjPg/TwZ5MWE6MLDD7tJXdHOE1Nb9QlYY0dxIZ/LGSXm267Jeq7whAp6pCFEqnfJABcunA7Ei8jFbVJP1G18tCrjqOr+26K0gcr4HdS/RR8VNnHdOnR7ZVX6k+81fehv80tz+7vufY2eV5umwSOsWUnUcO28NFwbvnfCQpImfEUndY+ykZzXSLhnPhdbpR+dm2v7FCg==");

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
      "AAAAAA4BAAAAAAAAMIIBCgKCAQEA7pNuopCQXfvUCCjJFKXgACIKu3bwGM77rCreRpv2+VW7mcv1tij1Q4Gsl5ArBfSSmMTWHhmLMhPJCR6MF8QGVlB10w49ED2OxNxoSz35VrUIkmbpCNqGXsySS/d4V0rjPQZUugPngBELJzvrNnwfinuWfK5cwZBLBEz0y/Ugcx1qdyXxvqmRfWhF/dP2IgGxm52gnRmSb78Mydzcgnaxf4808a+X6cp45VuEDFsRli4EIRte144V+fEFjt/gRyjUWXlqfYzPsBLaSaGfEqL8Oiy322/cNNaxsS74+n9tmMPOLGDtzEF01lGVBplKZx9OX6lD4JjnCyNOTuK2TWAbcQIDAQABAwAFAAQACwABAA=="
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
