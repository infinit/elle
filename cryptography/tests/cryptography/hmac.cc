#include "cryptography.hh"
#include "Sample.hh"

#include <cryptography/Exception.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/Plain.hh>
#include <cryptography/hmac.hh>
#include <cryptography/random.hh>

#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>
#include <elle/serialize/HexadecimalArchive.hh>
#include <elle/format/hexadecimal.hh>

static std::string const _input(
  "- Back off Susan Boyle!");

/*----------.
| Represent |
`----------*/

template <elle::Natural32 N,
          infinit::cryptography::Oneway O>
void
test_represent_n(elle::String const& K)
{
  // N)
  {
    infinit::cryptography::Digest digest =
      infinit::cryptography::hmac::sign(
        infinit::cryptography::Plain(_input),
        K,
        O);
    std::string representation =
      elle::format::hexadecimal::encode(digest.buffer());
    elle::printf("[representation %s] %s\n", N, representation);
  }
}

static
void
test_represent()
{
  // WARNING: To uncomment only if one wants to update the representations.
  return;

  // These generate hexadecimal-based representations which can be used in
  // other tests.

  // MD5.
  test_represent_n<1, infinit::cryptography::Oneway::md5>("one");
  // SHA.
  test_represent_n<2, infinit::cryptography::Oneway::sha>("deux");
  // SHA-1.
  test_represent_n<3, infinit::cryptography::Oneway::sha1>("san");
  // SHA-224.
  test_represent_n<4, infinit::cryptography::Oneway::sha224>("cetiri");
  // SHA-256.
  test_represent_n<5, infinit::cryptography::Oneway::sha256>("négy");
  // SHA-384.
  test_represent_n<6, infinit::cryptography::Oneway::sha384>("seis");
  // SHA-512.
  test_represent_n<7, infinit::cryptography::Oneway::sha512>("yedi");
}

/*--------.
| Operate |
`--------*/

template <infinit::cryptography::Oneway O,
          elle::Natural32 S>
void
test_operate_x(elle::String const& K,
               elle::String const& R)
{
  // Verify a HMAC digest.
  {
    elle::Buffer buffer = elle::format::hexadecimal::decode(R);

    BOOST_CHECK_EQUAL(
      infinit::cryptography::hmac::verify(
        infinit::cryptography::Digest(buffer),
        infinit::cryptography::Plain(_input),
        K,
        O),
      true);
  }

  // HMAC a complex type.
  {
    Sample const input(
      42,
      infinit::cryptography::random::generate<elle::String>(S));

    infinit::cryptography::Digest digest =
      infinit::cryptography::hmac::sign(input, K, O);

    BOOST_CHECK_EQUAL(
      infinit::cryptography::hmac::verify(
        digest,
        input,
        K,
        O),
      true);
  }

  // Try to verify an invalid HMAC.
  {
    elle::Buffer buffer =
      infinit::cryptography::random::generate<elle::Buffer>(16);

    BOOST_CHECK_EQUAL(
      infinit::cryptography::hmac::verify(
        infinit::cryptography::Digest(buffer),
        infinit::cryptography::Plain(_input),
        K,
        O),
      false);
  }
}

static
void
test_operate()
{
  // MD5 based on [representation 1].
  test_operate_x<infinit::cryptography::Oneway::md5, 820>("one",
                                                          "89f94dc2a07e5f98ccb0895faedb8597");
  // SHA based on [representation 2].
  test_operate_x<infinit::cryptography::Oneway::sha, 31>("deux",
                                                         "e9ccd70278f254a0d4a74e1806e8ca6fb0671e1a");
  // SHA-1 based on [representation 3].
  test_operate_x<infinit::cryptography::Oneway::sha1, 9028>("san",
                                                            "55362be05530fe6359fe980db363232fc02473c8");
  // SHA-224 based on [representation 4].
  test_operate_x<infinit::cryptography::Oneway::sha224, 630>("cetiri",
                                                             "b2dcadd118569cc8f7076bbbf9b13446cd7f0285b3b7a72d41911545");
  // SHA-256 based on [representation 5].
  test_operate_x<infinit::cryptography::Oneway::sha256, 73>("négy",
                                                            "6bead92026badb1e4102806a2277d143407a98a5c8265016e1b81b63b8436802");
  // SHA-384 based on [representation 6].
  test_operate_x<infinit::cryptography::Oneway::sha384, 933>("seis",
                                                             "10c4a8a6b5ab4168092a8c832d6076b7622bf541b8f2835556cfc6a5767d2c42ec7eb688ed12978ba0f1cfcd856837d9");
  // SHA-512 based on [representation 7].
  test_operate_x<infinit::cryptography::Oneway::sha512, 9223>("yedi",
                                                              "7bbc550b1459ac1b503c9ecb170863ef66ade07372c53f938cdf091d27b94d583282fc8d0a411fffcabbf1596ea2e42eef5dab879b3ec670adc2b6b277ce892c");
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("hmac");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_operate));

  boost::unit_test::framework::master_test_suite().add(suite);
}
