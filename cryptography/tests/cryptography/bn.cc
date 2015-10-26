#include "cryptography.hh"

#include <cryptography/bn.hh>
#include <cryptography/Error.hh>

#include <elle/serialization/json.hh>

#include <openssl/err.h>

/*----------.
| Represent |
`----------*/

static
void
test_represent()
{
  // WARNING: To uncomment only if one wants to update the representations.
  return;

  // These generate base64-based representations which can be used in
  // other tests.

  // 1)
  {
    ::BIGNUM bn;
    ::BN_init(&bn);
    if (::BN_pseudo_rand(&bn, 1024, -1, 0) == 0)
      throw infinit::cryptography::Error(
        elle::sprintf("unable to randomly generate a big number: %s",
                      ::ERR_error_string(ERR_get_error(), nullptr)));

    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      output.serialize("data", &bn);
    }

    ::BN_free(&bn);
    elle::printf("[representation 1] %s\n", stream.str());
  }
}

/*----------.
| Serialize |
`----------*/

static
void
test_serialize()
{
  // Deserialize from the hard-coded [representation 1]: useful for detecting
  // changes in formats.
  {
    std::string archive1(R"JSON({"data":"LxS0y2asiXs/vUKwfLzM7oMpbmwdKiHO6Rs4HUK2I6HbCNMxfRW0rd+ND/+UtWoa4bmhfYOcntYsZlkUTA65OSAPGVhMx4tyROtOAAo4e7mNu9A78ZEpFFOyHQfg9sOSbVIVq14+94D/w2TAbRCh/eKxfA0zXmvaI1AQZFlmQ80="})JSON");

    ::BIGNUM* bn = nullptr;

    std::stringstream stream1(archive1);
    typename elle::serialization::json::SerializerIn input1(stream1);
    input1.serialize("data", bn);

    std::stringstream stream2;
    {
      typename elle::serialization::json::SerializerOut output(stream2);
      output.serialize("data", bn);
    }

    ::BIGNUM* _bn = nullptr;

    typename elle::serialization::json::SerializerIn input2(stream2);
    input2.serialize("data", _bn);

    BOOST_CHECK_EQUAL(*bn, *_bn);

    ::BN_free(bn);
    ::BN_free(_bn);
  }
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("bn");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}

