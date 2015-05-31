#include "../cryptography.hh"
#include "../Sample.hh"

#include <cryptography/dsa/KeyPair.hh>
#include <cryptography/dsa/PublicKey.hh>
#include <cryptography/dsa/PrivateKey.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/Exception.hh>
#include <cryptography/random.hh>

#include <elle/printf.hh>
#include <elle/types.hh>
#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

/*----------.
| Represent |
`----------*/

static
void
_test_represent(elle::Natural32 const length,
                infinit::cryptography::Oneway const digest_algorithm)
{
  // 1)
  {
    infinit::cryptography::dsa::KeyPair keypair =
      infinit::cryptography::dsa::keypair::generate(length,
                                                    digest_algorithm);

    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << keypair;
    elle::printf("[representation 1] %s\n", archive);
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

  _test_represent(1024,
                  infinit::cryptography::Oneway::sha1);
}

/*---------.
| Generate |
`---------*/

static
infinit::cryptography::dsa::KeyPair
_test_generate(elle::Natural32 const length,
               infinit::cryptography::Oneway const digest_algorithm)
{
  infinit::cryptography::dsa::KeyPair keypair =
    infinit::cryptography::dsa::keypair::generate(length,
                                                  digest_algorithm);

  return (keypair);
}

static
void
test_generate()
{
  _test_generate(4096,
                 infinit::cryptography::Oneway::sha256);
}

/*----------.
| Construct |
`----------*/

static
void
test_construct()
{
  infinit::cryptography::dsa::KeyPair keypair1 =
    _test_generate(2048,
                   infinit::cryptography::Oneway::sha);

  // KeyPair copy.
  infinit::cryptography::dsa::KeyPair keypair2(keypair1);

  BOOST_CHECK_EQUAL(keypair1, keypair2);

  // KeyPair move.
  infinit::cryptography::dsa::KeyPair keypair3(std::move(keypair1));

  BOOST_CHECK_EQUAL(keypair2, keypair3);

  // Attributes copy.
  infinit::cryptography::dsa::KeyPair keypair4(keypair2.K(), keypair2.k());

  BOOST_CHECK_EQUAL(keypair2, keypair3);
  BOOST_CHECK_EQUAL(keypair2, keypair4);
  BOOST_CHECK_EQUAL(keypair3, keypair4);

  // Attributes move.
  infinit::cryptography::dsa::PublicKey K(keypair3.K());
  infinit::cryptography::dsa::PrivateKey k(keypair3.k());

  infinit::cryptography::dsa::KeyPair keypair5(std::move(K), std::move(k));

  BOOST_CHECK_EQUAL(keypair2, keypair3);
  BOOST_CHECK_EQUAL(keypair2, keypair4);
  BOOST_CHECK_EQUAL(keypair2, keypair5);
  BOOST_CHECK_EQUAL(keypair3, keypair4);
  BOOST_CHECK_EQUAL(keypair3, keypair5);
  BOOST_CHECK_EQUAL(keypair4, keypair5);
}

/*--------.
| Operate |
`--------*/

static
void
_test_operate(infinit::cryptography::dsa::KeyPair const& keypair)
{
  // Sign a plain text.
  {
    elle::String input =
      infinit::cryptography::random::generate<elle::String>(1493);
    infinit::cryptography::Signature signature =
      keypair.k().sign(
        infinit::cryptography::Plain{input});
    auto result =
      keypair.K().verify(signature,
                         infinit::cryptography::Plain{input});

    BOOST_CHECK_EQUAL(result, true);
  }

  // Sign a complex type.
  {
    Sample const input(
      84, infinit::cryptography::random::generate<elle::String>(10329));
    infinit::cryptography::Signature signature = keypair.k().sign(input);
    auto result = keypair.K().verify(signature, input);

    BOOST_CHECK_EQUAL(result, true);
  }
}

static
void
test_operate()
{
  infinit::cryptography::dsa::KeyPair keypair =
    _test_generate(512,
                   infinit::cryptography::Oneway::md5);

  _test_operate(keypair);
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
    infinit::cryptography::dsa::KeyPair keypair1 =
      _test_generate(1024,
                     infinit::cryptography::Oneway::sha512);

    elle::String archive;
    elle::serialize::to_string(archive) << keypair1;

    auto extractor = elle::serialize::from_string(archive);

    infinit::cryptography::dsa::KeyPair keypair2(extractor);

    BOOST_CHECK_EQUAL(keypair1, keypair2);

    _test_operate(keypair2);
  }

  // For every hard-coded strings [representation 1] in every format,
  // deserialize the key pairs, re-serialize them, make sure they can
  // be used and finally upgrade them: useful for detecting changes in formats.
  {
    std::vector<elle::String> const archives{
      // format 0
      "AAAAAAAApgEAAAAAAAAwggGiAoGBAOmtrh0566Wu8Ge1c5caV7y9Kj/CmyjLAbhn7td6crPfEn1Q+pn1KuNaBNmhZPgLdjLw/g5n39DExYqc8hfOekTInkNWWpZK/VEhzhZ41a9qIh7Rw7X/J95wOMdbEYhVWpDXpXxdtZQpdiqRAr5iV8rgcNopYZ0Hn6hJ5iUMNiQCAoGBAPYwBRFPaqPWkQRpJ9UhH6rZ8hjqI0n6osjzx4kkwPoNnN/LhzBjFZYE+otKW4tEObvS8z+WMdV9rrCNPi6TjSjXJwoBfn51JPjXgKz83lOSy/mjWcQdGVFp6mDSy21o5juqfuu78+K1Qi8jAkqt0YvCpvkTJfae6mP6A9Y/xjHrAhUAnmSNTdr6Npof5hA+BlAMdFd4CiMCgYBBusC/3HKxzGu7xffT6AnGLxM2sH4IgyLD9oD0LHu4qjSvCjsuuQ408NPQteNCE/h/F4rnq7UWwBnL7uC2LZZ4Y8/8x0/M5TcNfwh56wy2DlnIM7g8IxiCTJ86aPYU7bqJdxWM66kZ0G+LHWSZvs8aYDeB7EQPss6nlObLuYC6LgIAAAAAAL8BAAAAAAAAMIIBuwIBAAKBgQD2MAURT2qj1pEEaSfVIR+q2fIY6iNJ+qLI88eJJMD6DZzfy4cwYxWWBPqLSluLRDm70vM/ljHVfa6wjT4uk40o1ycKAX5+dST414Cs/N5Tksv5o1nEHRlRaepg0sttaOY7qn7ru/PitUIvIwJKrdGLwqb5EyX2nupj+gPWP8Yx6wIVAJ5kjU3a+jaaH+YQPgZQDHRXeAojAoGAQbrAv9xyscxru8X30+gJxi8TNrB+CIMiw/aA9Cx7uKo0rwo7LrkONPDT0LXjQhP4fxeK56u1FsAZy+7gti2WeGPP/MdPzOU3DX8IeesMtg5ZyDO4PCMYgkyfOmj2FO26iXcVjOupGdBvix1kmb7PGmA3gexED7LOp5Tmy7mAui4CgYEA6a2uHTnrpa7wZ7VzlxpXvL0qP8KbKMsBuGfu13pys98SfVD6mfUq41oE2aFk+At2MvD+Dmff0MTFipzyF856RMieQ1Zalkr9USHOFnjVr2oiHtHDtf8n3nA4x1sRiFVakNelfF21lCl2KpECvmJXyuBw2ilhnQefqEnmJQw2JAICFCH+rQAmB2cw0guwMDinVd6z4IXpAgA="
      };

    infinit::cryptography::test::formats<infinit::cryptography::dsa::KeyPair>(
      archives, std::bind(_test_operate, std::placeholders::_1));
  }
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("dsa/KeyPair");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}
