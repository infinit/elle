#include "cryptography.hh"

#include <elle/cryptography/bn.hh>
#include <openssl/bn.h>

#include <elle/cryptography/Error.hh>

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
      throw elle::cryptography::Error(
        elle::sprintf("unable to randomly generate a big number: %s",
                      ::ERR_error_string(ERR_get_error(), nullptr)));

    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      output.serialize("data", &bn);
    }

    ::BN_free(&bn);
    elle::fprintf(std::cout, "[representation 1] %s\n", stream.str());
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

static
void
test_compare()
{
  ::BIGNUM bn_1000;
  ::BIGNUM bn_2000;
  ::BIGNUM bn_minus_1000;

  ::BN_init(&bn_1000);
  ::BN_set_word(&bn_1000, 1000);

  ::BN_init(&bn_2000);
  ::BN_set_word(&bn_2000, 2000);

  ::BN_init(&bn_minus_1000);
  ::BN_set_word(&bn_minus_1000, 1000);
  ::BN_set_negative(&bn_minus_1000, 1);

#define CHECK_EQUAL(x)                   \
  BOOST_CHECK_EQUAL(x, x);               \
  BOOST_CHECK_GE(x, x);                  \
  BOOST_CHECK_LE(x, x)

  CHECK_EQUAL(bn_1000);
  CHECK_EQUAL(bn_2000);
  CHECK_EQUAL(bn_minus_1000);
#undef CHECK_EQUAL

#define CHECK_LESSER(x, y)                      \
  BOOST_CHECK_LT(x, y);                         \
  BOOST_CHECK_LE(x, y)

  CHECK_LESSER(bn_minus_1000, bn_1000);
  CHECK_LESSER(bn_minus_1000, bn_2000);
  CHECK_LESSER(bn_1000, bn_2000);
#undef CHECK_LESSER

#define CHECK_GREATER(x, y)                     \
  BOOST_CHECK_GT(x, y);                         \
  BOOST_CHECK_GE(x, y)

  CHECK_GREATER(bn_2000, bn_minus_1000);
  CHECK_GREATER(bn_2000, bn_1000);
  CHECK_GREATER(bn_1000, bn_minus_1000);
#undef CHECK_GREATER

  ::BN_free(&bn_1000);
  ::BN_free(&bn_2000);
  ::BN_free(&bn_minus_1000);
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("bn");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_serialize));
  suite->add(BOOST_TEST_CASE(test_compare));


  boost::unit_test::framework::master_test_suite().add(suite);
}
