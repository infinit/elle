#include "cryptography.hh"
#include "Sample.hh"

#include <cryptography/Exception.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/hmac.hh>
#include <cryptography/random.hh>

#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>
#include <elle/serialize/HexadecimalArchive.hh>
#include <elle/format/hexadecimal.hh>

static std::string const _input(
  "- Do you think she's expecting something big?"
  "- You mean, like anal?");

/*----------.
| Represent |
`----------*/

template <elle::Natural32 N,
          infinit::cryptography::Oneway O>
void
test_represent_n()
{
  // N)
  {
    infinit::cryptography::Digest key(elle::Buffer("key", 3));
    infinit::cryptography::Digest digest =
      hmac(infinit::cryptography::Plain(_input), key, O);
    std::string representation =
      elle::format::hexadecimal::encode(digest.buffer());
    elle::sprintf("[representation %s] %s\n", N, representation);
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
  test_represent_n<1, infinit::cryptography::Oneway::md5>();
  // SHA.
  test_represent_n<2, infinit::cryptography::Oneway::sha>();
  // SHA-1.
  test_represent_n<3, infinit::cryptography::Oneway::sha1>();
  // SHA-224.
  test_represent_n<4, infinit::cryptography::Oneway::sha224>();
  // SHA-256.
  test_represent_n<5, infinit::cryptography::Oneway::sha256>();
  // SHA-384.
  test_represent_n<6, infinit::cryptography::Oneway::sha384>();
  // SHA-512.
  test_represent_n<7, infinit::cryptography::Oneway::sha512>();
}

/*--------.
| Operate |
`--------*/

template <infinit::cryptography::Oneway O,
          elle::Natural32 S>
void
test_operate_x(elle::String const& R)
{
  infinit::cryptography::Digest key(elle::Buffer("key", 3));

  // HMAC a plain.
  {
    infinit::cryptography::Digest digest1 =
      infinit::cryptography::hmac(infinit::cryptography::Plain(_input), key, O);
    infinit::cryptography::Digest digest2 =
      infinit::cryptography::hmac(infinit::cryptography::Plain(_input), key, O);

    BOOST_CHECK_EQUAL(digest1, digest2);

    elle::Buffer buffer = elle::format::hexadecimal::decode(R);

    BOOST_CHECK_EQUAL(digest1.buffer(), buffer);
  }

  // HMAC a complex type.
  {
    Sample const input(
      42, infinit::cryptography::random::generate<elle::String>(S));

    infinit::cryptography::Digest digest1 =
      infinit::cryptography::hmac(input, key, O);
    infinit::cryptography::Digest digest2 =
      infinit::cryptography::hmac(input, key, O);

    BOOST_CHECK_EQUAL(digest1, digest2);
  }
}

static
void
test_operate()
{
  // MD5 based on [representation 1].
  test_operate_x<infinit::cryptography::Oneway::md5, 820>("f580b9c649f64331bd0add676e13e03d");
  // SHA based on [representation 2].
  test_operate_x<infinit::cryptography::Oneway::sha, 31>("8f23a8944e8abf5062743cc8604ab1b37c4ed2d1");
  // SHA-1 based on [representation 3].
  test_operate_x<infinit::cryptography::Oneway::sha1, 9028>("ad149753940c4a9e3faf56e4a7125acd28f0454f");
  // SHA-224 based on [representation 4].
  test_operate_x<infinit::cryptography::Oneway::sha224, 630>("1293b424873159446fd0c23e1f0367f186591b7acaadd2668b492c10");
  // SHA-256 based on [representation 5].
  test_operate_x<infinit::cryptography::Oneway::sha256, 73>("ea95ff42cb433311cadac495946fc03fd370f42c4f7214b56d6e5fb02029ad3b");
  // SHA-384 based on [representation 6].
  test_operate_x<infinit::cryptography::Oneway::sha384, 933>("83461c33c0f18386dd2092a9937c07205db402a5c0c520981804fb5f893f5fe13f8ef6dfeb7490a44835305d3184989e");
  // SHA-512 based on [representation 7].
  test_operate_x<infinit::cryptography::Oneway::sha512, 9223>("74ea6d290be470f30784675e2798376e662669189758afd03fd24d917152dba29f95f35e27d167a6dd1b7be28fdfe8ae68d54aa70ba6c90b03b0e3c8676b5c1d");
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
