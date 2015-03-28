#include "cryptography.hh"
#include "Sample.hh"

#include <cryptography/Exception.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/hash.hh>
#include <cryptography/random.hh>

#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>
#include <elle/serialize/HexadecimalArchive.hh>
#include <elle/format/hexadecimal.hh>

static std::string const _input(
  "- Do you think she's expecting something big?"
  "- You mean, like anal?");

//
// ---------- hash ------------------------------------------------------------
//

/*----------.
| Represent |
`----------*/

template <elle::Natural32 N,
          infinit::cryptography::Oneway O>
void
test_hash_represent_n()
{
  // N)
  {
    infinit::cryptography::Digest digest =
      infinit::cryptography::hash(
        infinit::cryptography::Plain(_input),
        O);
    std::string representation =
      elle::format::hexadecimal::encode(digest.buffer());
    elle::sprintf("[representation %s] %s\n", N, representation);
  }
}

static
void
test_hash_represent()
{
  // WARNING: To uncomment only if one wants to update the representations.
  return;

  // These generate hexadecimal-based representations which can be used in
  // other tests.

  // MD5.
  test_hash_represent_n<1, infinit::cryptography::Oneway::md5>();
  // SHA.
  test_hash_represent_n<2, infinit::cryptography::Oneway::sha>();
  // SHA-1.
  test_hash_represent_n<3, infinit::cryptography::Oneway::sha1>();
  // SHA-224.
  test_hash_represent_n<4, infinit::cryptography::Oneway::sha224>();
  // SHA-256.
  test_hash_represent_n<5, infinit::cryptography::Oneway::sha256>();
  // SHA-384.
  test_hash_represent_n<6, infinit::cryptography::Oneway::sha384>();
  // SHA-512.
  test_hash_represent_n<7, infinit::cryptography::Oneway::sha512>();
}

/*--------.
| Operate |
`--------*/

template <infinit::cryptography::Oneway O,
          elle::Natural32 S>
void
test_hash_operate_x(elle::String const& R)
{
  // Hash a plain.
  {
    infinit::cryptography::Digest digest1 =
      infinit::cryptography::hash(
        infinit::cryptography::Plain{_input},
        O);
    infinit::cryptography::Digest digest2 =
      infinit::cryptography::hash(
        infinit::cryptography::Plain{_input},
        O);

    BOOST_CHECK_EQUAL(digest1, digest2);

    elle::Buffer buffer = elle::format::hexadecimal::decode(R);

    BOOST_CHECK_EQUAL(digest1.buffer(), buffer);
  }

  // Hash a complex type.
  {
    Sample const input(
      42, infinit::cryptography::random::generate<elle::String>(S));

    infinit::cryptography::Digest digest1 =
      infinit::cryptography::hash(input, O);
    infinit::cryptography::Digest digest2 =
      infinit::cryptography::hash(input, O);

    BOOST_CHECK_EQUAL(digest1, digest2);
  }
}

static
void
test_hash_operate()
{
  // MD5 based on [representation 1].
  test_hash_operate_x<infinit::cryptography::Oneway::md5, 820>("951b3e45b757840730c0e222eb71091f");
  // SHA based on [representation 2].
  test_hash_operate_x<infinit::cryptography::Oneway::sha, 31>("077e1b57df114df2bba973d0777fb3b3718160a9");
  // SHA-1 based on [representation 3].
  test_hash_operate_x<infinit::cryptography::Oneway::sha1, 9028>("2cc1e684750e1fc377986a351d344577abdb9979");
  // SHA-224 based on [representation 4].
  test_hash_operate_x<infinit::cryptography::Oneway::sha224, 630>("e43c1f9e51952af2ee7220d4c11e9fbb3392d830f20389c2783a19fd");
  // SHA-256 based on [representation 5].
  test_hash_operate_x<infinit::cryptography::Oneway::sha256, 73>("3a88f414cb608e823bb1a72214267f5e0f1e5c95858735e7f3d99989d7baa1e2");
  // SHA-384 based on [representation 6].
  test_hash_operate_x<infinit::cryptography::Oneway::sha384, 933>("ae32f347e07266c48efa5f8878aaf31fb0eb7b75d999e2b6fc648f5bf4e04cc050e95c33503920ce396a68a50e81aff8");
  // SHA-512 based on [representation 7].
  test_hash_operate_x<infinit::cryptography::Oneway::sha512, 9223>("8bae3559218793c6634d17867d948069ecbc0a2d6b6d7faeb1b37b0cb14cda18e3375225809d9b0e2a68a68b65818fbe3e737a76537e55de8c288cf7cf52d47b");
}

//
// ---------- hmac ------------------------------------------------------------
//

/*----------.
| Represent |
`----------*/

template <elle::Natural32 N,
          infinit::cryptography::Oneway O>
void
test_hmac_represent_n()
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
test_hmac_represent()
{
  // WARNING: To uncomment only if one wants to update the representations.
  return;

  // These generate hexadecimal-based representations which can be used in
  // other tests.

  // MD5.
  test_hmac_represent_n<1, infinit::cryptography::Oneway::md5>();
  // SHA.
  test_hmac_represent_n<2, infinit::cryptography::Oneway::sha>();
  // SHA-1.
  test_hmac_represent_n<3, infinit::cryptography::Oneway::sha1>();
  // SHA-224.
  test_hmac_represent_n<4, infinit::cryptography::Oneway::sha224>();
  // SHA-256.
  test_hmac_represent_n<5, infinit::cryptography::Oneway::sha256>();
  // SHA-384.
  test_hmac_represent_n<6, infinit::cryptography::Oneway::sha384>();
  // SHA-512.
  test_hmac_represent_n<7, infinit::cryptography::Oneway::sha512>();
}

/*--------.
| Operate |
`--------*/

template <infinit::cryptography::Oneway O,
          elle::Natural32 S>
void
test_hmac_operate_x(elle::String const& R)
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
test_hmac_operate()
{
  // MD5 based on [representation 1].
  test_hmac_operate_x<infinit::cryptography::Oneway::md5, 820>("f580b9c649f64331bd0add676e13e03d");
  // SHA based on [representation 2].
  test_hmac_operate_x<infinit::cryptography::Oneway::sha, 31>("8f23a8944e8abf5062743cc8604ab1b37c4ed2d1");
  // SHA-1 based on [representation 3].
  test_hmac_operate_x<infinit::cryptography::Oneway::sha1, 9028>("ad149753940c4a9e3faf56e4a7125acd28f0454f");
  // SHA-224 based on [representation 4].
  test_hmac_operate_x<infinit::cryptography::Oneway::sha224, 630>("1293b424873159446fd0c23e1f0367f186591b7acaadd2668b492c10");
  // SHA-256 based on [representation 5].
  test_hmac_operate_x<infinit::cryptography::Oneway::sha256, 73>("ea95ff42cb433311cadac495946fc03fd370f42c4f7214b56d6e5fb02029ad3b");
  // SHA-384 based on [representation 6].
  test_hmac_operate_x<infinit::cryptography::Oneway::sha384, 933>("83461c33c0f18386dd2092a9937c07205db402a5c0c520981804fb5f893f5fe13f8ef6dfeb7490a44835305d3184989e");
  // SHA-512 based on [representation 7].
  test_hmac_operate_x<infinit::cryptography::Oneway::sha512, 9223>("74ea6d290be470f30784675e2798376e662669189758afd03fd24d917152dba29f95f35e27d167a6dd1b7be28fdfe8ae68d54aa70ba6c90b03b0e3c8676b5c1d");
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();

  suite.add(BOOST_TEST_CASE(test_hash_represent));
  suite.add(BOOST_TEST_CASE(test_hash_operate));

  suite.add(BOOST_TEST_CASE(test_hmac_represent));
  suite.add(BOOST_TEST_CASE(test_hmac_operate));
}
