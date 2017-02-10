#include "../cryptography.hh"

#include <cryptography/dsa/PublicKey.hh>
#include <cryptography/dsa/PrivateKey.hh>
#include <cryptography/dsa/KeyPair.hh>

#include <elle/serialization/json.hh>

/*----------.
| Represent |
`----------*/

static std::string const _input("my balls are ok!");

static
void
_test_represent()
{
  elle::cryptography::dsa::KeyPair keypair =
    elle::cryptography::dsa::keypair::generate(2048);

  // 1)
  {
    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      elle::cryptography::dsa::PublicKey K = keypair.K();
      K.serialize(output);
    }

    elle::fprintf(std::cout, "[representation 1] %s\n", stream.str());
  }

  // 2)
  {
    elle::Buffer signature = keypair.k().sign(_input);

    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      output.serialize("signature", signature);
    }

    elle::fprintf(std::cout, "[representation 2] %s\n", stream.str());
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
elle::cryptography::dsa::PublicKey
_test_generate(uint32_t const length = 1024)
{
  elle::cryptography::dsa::KeyPair keypair =
    elle::cryptography::dsa::keypair::generate(length);

  elle::cryptography::dsa::PublicKey K(keypair.K());

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
  elle::cryptography::dsa::PublicKey K1 = _test_generate(2048);

  // PublicKey copy.
  elle::cryptography::dsa::PublicKey K2(K1);

  BOOST_CHECK_EQUAL(K1, K2);

  // PublicKey move.
  elle::cryptography::dsa::PublicKey K3(std::move(K1));

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
  std::string representation(R"JSON({"digest algorithm":4,"dsa":"MIIDJQKCAQEAq+VVghywf5oodq7USwkzijdlzH53p+OUGfORgcYuiv5mTnDMgCpzQgXZtRGI5y1tmtQcnd1Kc4rokKl5kerQcu8z9h0EAefwIVM98USosBeziVowC25jOlkuM4F1QzDyMOmdN9Bc1EBKXaqxHGtRBKSpKhi5m/UV7xX3DgrHOcrqnyXw0THJ+YjamzKvI11oS22hJHksl9Gryxs6t78YJV9AaPKbgtcAM8vK0qmKAHXi0uZhLtwZelv2ukzGbJwSlJ2Zx7fHS1+fNvu+/iThBYTzkuInmZeaPpoVYFa7BOppqqL91DPE/HO3lW8l4mPw5l2X7c8TStCMDtPB3qb2kwKCAQEAto+zhq1jMPrUTW0SPaJGvcSeyDRCbu5FNdQIOxBEQSDI1XxZ/XboY9Q4+uaGxl3AfU90P8E5wDtlLv19hCc2cdoNHZ/kZh+Zi9Yhi1HM9RGN0SmAAvIYI5smoDej0sqKHp2e4yz0tzxD/stemxYQwIK54pXKg7o/pc1mOeFtlPbZEMPwU8t1raDOFvE595uBqspYG68Bj77icTgNHszaCTBCpXzcMLWj08HnmU5leFZ8aaEtPxeKEukYhRYpxBP7U3NKBVRgZ5dTv+9/2rLzgj73DUz8xTQRsWnXEPRbct6pN0WZMEjyigysonGsilKWsVfiExOYyLuxrsVbO6MPGwIVAPR6HJ/qfhGuFWeKv0rE4dCTljBFAoIBAFHiseJEDGA9mi+E/ShFXv0BEb5FiDwrP10n+fT/InBLDpxW17r0mf25p5oXoD7tLXlQbyZoM2qzApGtQLKiQCgpphsMny/BfalbxD0LtwrRt1zAVaZnTQAU6pFFqDJnZUdm7CRvf0SgCfAW0SYGOrodxkdFmL5mqKZLGxjsrTqcLQAn5uHaJOSBNAyewwc5/fsYvfWOyH9CgUqqJ3XrLN0lEguRn9DFl3b2OuwCBDKI8Hty8sO24u8HgJx/n/Ng1xzlIh+MaN62PaW5KjaYqZvKah+2LG9O3s9rlg2WFYKz9uUCToj6d5lzF9RtxKYJVjk8MWsi3AM0lemzedaRXx0="})JSON");

  std::stringstream stream(representation);
  typename elle::serialization::json::SerializerIn input(stream);
  elle::cryptography::dsa::PublicKey K(input);

  // The following operations are based on hard-coded base64 string which
  // represent the data on which to operate, in their serialized form.

  // Verify plain from [representation 2].
  {
    std::string archive(R"JSON({"signature":"MC0CFFOtibJJmEq4lq0DxGv/zRMPNEC+AhUA0GzN1zjs1NCmT78Cm8bp9c8zeYg="})JSON");

    std::stringstream stream(archive);
    typename elle::serialization::json::SerializerIn input(stream);
    elle::Buffer signature;
    input.serialize("signature", signature);

    auto result = K.verify(signature, _input);

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
  elle::cryptography::dsa::PublicKey K1 = _test_generate(1024);
  elle::cryptography::dsa::PublicKey K2 = _test_generate(1024);

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
    elle::cryptography::dsa::PublicKey K1 = _test_generate(2048);

    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      K1.serialize(output);
    }

    typename elle::serialization::json::SerializerIn input(stream);
    elle::cryptography::dsa::PublicKey K2(input);

    BOOST_CHECK_EQUAL(K1, K2);
  }

  // For every hard-coded strings [representation 1] in every format,
  // deserialize the public keys, re-serialize them, make sure they can
  // be used and finally upgrade them: useful for detecting changes in formats.
  {
    std::vector<std::string> const archives{
      // format 0
      R"JSON({"digest algorithm":4,"dsa":"MIIDJQKCAQEAq+VVghywf5oodq7USwkzijdlzH53p+OUGfORgcYuiv5mTnDMgCpzQgXZtRGI5y1tmtQcnd1Kc4rokKl5kerQcu8z9h0EAefwIVM98USosBeziVowC25jOlkuM4F1QzDyMOmdN9Bc1EBKXaqxHGtRBKSpKhi5m/UV7xX3DgrHOcrqnyXw0THJ+YjamzKvI11oS22hJHksl9Gryxs6t78YJV9AaPKbgtcAM8vK0qmKAHXi0uZhLtwZelv2ukzGbJwSlJ2Zx7fHS1+fNvu+/iThBYTzkuInmZeaPpoVYFa7BOppqqL91DPE/HO3lW8l4mPw5l2X7c8TStCMDtPB3qb2kwKCAQEAto+zhq1jMPrUTW0SPaJGvcSeyDRCbu5FNdQIOxBEQSDI1XxZ/XboY9Q4+uaGxl3AfU90P8E5wDtlLv19hCc2cdoNHZ/kZh+Zi9Yhi1HM9RGN0SmAAvIYI5smoDej0sqKHp2e4yz0tzxD/stemxYQwIK54pXKg7o/pc1mOeFtlPbZEMPwU8t1raDOFvE595uBqspYG68Bj77icTgNHszaCTBCpXzcMLWj08HnmU5leFZ8aaEtPxeKEukYhRYpxBP7U3NKBVRgZ5dTv+9/2rLzgj73DUz8xTQRsWnXEPRbct6pN0WZMEjyigysonGsilKWsVfiExOYyLuxrsVbO6MPGwIVAPR6HJ/qfhGuFWeKv0rE4dCTljBFAoIBAFHiseJEDGA9mi+E/ShFXv0BEb5FiDwrP10n+fT/InBLDpxW17r0mf25p5oXoD7tLXlQbyZoM2qzApGtQLKiQCgpphsMny/BfalbxD0LtwrRt1zAVaZnTQAU6pFFqDJnZUdm7CRvf0SgCfAW0SYGOrodxkdFmL5mqKZLGxjsrTqcLQAn5uHaJOSBNAyewwc5/fsYvfWOyH9CgUqqJ3XrLN0lEguRn9DFl3b2OuwCBDKI8Hty8sO24u8HgJx/n/Ng1xzlIh+MaN62PaW5KjaYqZvKah+2LG9O3s9rlg2WFYKz9uUCToj6d5lzF9RtxKYJVjk8MWsi3AM0lemzedaRXx0="})JSON"
    };

    elle::cryptography::test::formats<
      elle::cryptography::dsa::PublicKey>(archives);
  }
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("dsa/PublicyKey");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_compare));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}
