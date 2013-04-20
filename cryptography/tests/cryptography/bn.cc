#include "cryptography.hh"

#include <cryptography/bn.hh>
#include <cryptography/Exception.hh>

#include <elle/serialize/Base64Archive.hh>
#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

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
      throw infinit::cryptography::Exception(
        elle::sprintf("unable to randomly generate a big number: %s",
                      ::ERR_error_string(ERR_get_error(), nullptr)));

    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << bn;
    elle::printf("[representation 1] %s\n", archive);
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
    elle::String archive1("AACAAAAA8DGAteEiGfhKAnwaPaIUxWWrkOxUQs7RMq2emyWnc2aPpjWY+PwcSFP1nsz4FU7hFAK3l/dh9p1uw50vp6mzhXZdXAAAqkiaHRqrJ2ckfj7mPF9ikKk88dmSmqZGsnFtVhuD2/bOGbdgGPzSRnJ8vNj77zzxRAMF88vwjliHR2s=");

    ::BIGNUM bn;
    ::BN_init(&bn);

    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(archive1) >> bn;

    elle::String archive2;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive2) << bn;

    BOOST_CHECK_EQUAL(archive1, archive2);
  }

  // Deserialize an invalid representation.
  {
    elle::String archive("EWDqiqwd9032eajnoIAijfwofqaaaodQEJFAOW");

    ::BIGNUM bn;
    ::BN_init(&bn);

    BOOST_CHECK_THROW(elle::serialize::from_string<
                        elle::serialize::InputBase64Archive>(archive) >> bn,
                      elle::serialize::Exception);
  }
}

/*-----.
| Main |
`-----*/

static
bool
test()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("bn");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);

  return (true);
}

int
main(int argc,
     char** argv)
{
  return (boost::unit_test::unit_test_main(test, argc, argv));
}
