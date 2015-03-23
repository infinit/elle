#include "../cryptography.hh"
#include "../Sample.hh"

#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/keypair.hh>
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
        infinit::cryptography::Plain{
          elle::ConstWeakBuffer{_input1}});
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
  elle::String representation("AAAAAA4BAAAAAAAAMIIBCgKCAQEA1CJdc0qVxKAtvMPQ0T3i4H1CRiVoJZDakX2YJuzADqQeJpMbUvXQnmkTJ5CMf3gBRdnkz++LSfM2bG122U7SWryuZIEnB9SXE5P/gDMxt50AuVEvXxKiCaw9Y3IN/lQaT9e19mtl4zb2ZZSYQKzOvPL190MyQDk+9hDKsf7cnS99kbdeOM8PeqRoMbr8Brs3ae2zgezl6K+j1mQrkXgxJW7NqeH0Y7r1a2HHE8jkARnC3iAEmm/ZxRK3FF4JJTNqV1UKzVl4sIZ0qsIfesCsBZZTZ0oWpA/ed/GjWAbHd39TZ1Nh+hch9lOzWnZkMnG1Q1Fw5ID7lZtxD4JQNHnxLQIDAQAB");

  auto extractor =
    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(representation);
  infinit::cryptography::rsa::PublicKey K(extractor);

  // The following operations are based on hard-coded base64 string which
  // represent the data on which to operate, in their serialized form.

  // Verify plain from [representation 2].
  {
    elle::String archive("AAAAAAABAAAAAAAALKe9gjbFAxgkZgLTDsIq55hW6OOe1Z5foYSVjWV0fP+JBXe4pd/AYDqq/z3rhm5B5iVWusBs7ZFaTiS0PPrtBiIvLBq/qSt5kKERzi76n263zh2rVPAndu+HB+7Ok5jGwWEzTGxFd7PpGXHP7lIpkvdNPiraecb9Oe3dTvYuKQ1JQ3YRT32+uw/E+3pwLektxHv1yN6c05INYLhPtmX5iRfdZ6lsI6bnyejT9wp0IgthVfHmGDLi5QfCNeJjzPLNHLBrfhd1MzODH6zHtAqtF8r2dWVCWYA/lKjlFwoCdD5WdL+18faLTrquEtPJyxa2rI0LxB3ZMrvEkc0NL+siUQ==");

    auto extractor =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive);
    infinit::cryptography::Signature signature(extractor);

    auto result =
      K.verify(signature,
               infinit::cryptography::Plain{
                 elle::ConstWeakBuffer{_input1}});

    BOOST_CHECK_EQUAL(result, true);
  }

  // Verify plain from [representation 3].
  {
    elle::String archive("AAAAAAABAAAAAAAAetN5D0kOORVokGuT61o3EbwezS+KlBzqrD2I5CjIQ4n1XWTafwy+xjmCJzqbsJvhgLpmRf15d1bphtbWU0M6XnlUzODkEXYEoQbRvtlnAIpwvoAS0lFb51n6uBonutpIWR/eW/PEjuAaCkS9BZ6VFWTVkeOp7RHkRAF7JxlAqN933Lxmu1Z+eIIpVxLl1o15Ia7vom9q9/DqhLfl55Mz89G5O+56G+SCNP56QXVuGKpfj/nw7Ri2T6Q0HwqJ+oFcrbFStyL2ZJJLcJnHNWkTH2O04MFUK4/Z3UQrsVHjz1IhmZW4fJ6EhcRCL/yyvMgWkC/aKhXQltlZPi+T1wZe2w==");

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

  if (K1 < K2)
  {
    BOOST_CHECK(K1 <= K2);
    BOOST_CHECK(!(K1 > K2));
    BOOST_CHECK(!(K1 >= K2));
  }
  else
  {
    BOOST_CHECK(K1 >= K2);
    BOOST_CHECK(!(K1 < K2));
    BOOST_CHECK(!(K1 <= K2));
  }
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
      "AAAAAA4BAAAAAAAAMIIBCgKCAQEA1CJdc0qVxKAtvMPQ0T3i4H1CRiVoJZDakX2YJuzADqQeJpMbUvXQnmkTJ5CMf3gBRdnkz++LSfM2bG122U7SWryuZIEnB9SXE5\
P/gDMxt50AuVEvXxKiCaw9Y3IN/lQaT9e19mtl4zb2ZZSYQKzOvPL190MyQDk+9hDKsf7cnS99kbdeOM8PeqRoMbr8Brs3ae2zgezl6K+j1mQrkXgxJW7NqeH0Y7r1a2HHE8jkARnC3iAEmm/ZxRK3FF4JJTNqV1UKzVl4sIZ0qsIfesCsBZZTZ0oWpA/ed/GjWAbHd39TZ1Nh+hch9lOzWnZkMnG1Q1Fw5ID7lZtxD4JQNHnxLQIDAQAB"
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
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("PublicyKey");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_compare));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}

