#include "../cryptography.hh"

#include <cryptography/rsa/KeyPair.hh>
#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/Padding.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/Error.hh>
#include <cryptography/random.hh>

#include <elle/printf.hh>
#include <elle/types.hh>
#include <elle/serialization/json.hh>

/*----------.
| Represent |
`----------*/

static
void
_test_represent(elle::Natural32 const length,
                infinit::cryptography::rsa::Padding const encryption_padding,
                infinit::cryptography::rsa::Padding const signature_padding,
                infinit::cryptography::Oneway const digest_algorithm,
                infinit::cryptography::Cipher const envelope_cipher,
                infinit::cryptography::Mode const envelope_mode)
{
  // 1)
  {
    infinit::cryptography::rsa::KeyPair keypair =
      infinit::cryptography::rsa::keypair::generate(length,
                                                    encryption_padding,
                                                    signature_padding,
                                                    digest_algorithm,
                                                    envelope_cipher,
                                                    envelope_mode);

    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      keypair.serialize(output);
    }

    elle::printf("[representation 1] %s\n", stream.str());
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
                  infinit::cryptography::rsa::Padding::oaep,
                  infinit::cryptography::rsa::Padding::pss,
                  infinit::cryptography::Oneway::sha1,
                  infinit::cryptography::Cipher::idea,
                  infinit::cryptography::Mode::cbc);
}

/*---------.
| Generate |
`---------*/

static
infinit::cryptography::rsa::KeyPair
_test_generate(elle::Natural32 const length,
               infinit::cryptography::rsa::Padding const encryption_padding,
               infinit::cryptography::rsa::Padding const signature_padding,
               infinit::cryptography::Oneway const digest_algorithm,
               infinit::cryptography::Cipher const envelope_cipher,
               infinit::cryptography::Mode const envelope_mode)
{
  infinit::cryptography::rsa::KeyPair keypair =
    infinit::cryptography::rsa::keypair::generate(length,
                                                  encryption_padding,
                                                  signature_padding,
                                                  digest_algorithm,
                                                  envelope_cipher,
                                                  envelope_mode);

  return (keypair);
}

static
void
test_generate()
{
  _test_generate(4096,
                 infinit::cryptography::rsa::Padding::pkcs1,
                 infinit::cryptography::rsa::Padding::pkcs1,
                 infinit::cryptography::Oneway::sha256,
                 infinit::cryptography::Cipher::blowfish,
                 infinit::cryptography::Mode::ecb);
}

/*----------.
| Construct |
`----------*/

static
void
test_construct()
{
  infinit::cryptography::rsa::KeyPair keypair1 =
    _test_generate(2048,
                   infinit::cryptography::rsa::Padding::oaep,
                   infinit::cryptography::rsa::Padding::pkcs1,
                   infinit::cryptography::Oneway::sha,
                   infinit::cryptography::Cipher::aes128,
                   infinit::cryptography::Mode::ofb);

  // KeyPair copy.
  infinit::cryptography::rsa::KeyPair keypair2(keypair1);

  BOOST_CHECK_EQUAL(keypair1, keypair2);

  // KeyPair move.
  infinit::cryptography::rsa::KeyPair keypair3(std::move(keypair1));

  BOOST_CHECK_EQUAL(keypair2, keypair3);

  // Attributes copy.
  infinit::cryptography::rsa::KeyPair keypair4(keypair2.K(), keypair2.k());

  BOOST_CHECK_EQUAL(keypair2, keypair3);
  BOOST_CHECK_EQUAL(keypair2, keypair4);
  BOOST_CHECK_EQUAL(keypair3, keypair4);

  // Attributes move.
  infinit::cryptography::rsa::PublicKey K(keypair3.K());
  infinit::cryptography::rsa::PrivateKey k(keypair3.k());

  infinit::cryptography::rsa::KeyPair keypair5(std::move(K), std::move(k));

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
_test_operate(infinit::cryptography::rsa::KeyPair const& keypair)
{
  // Public/private seal/open.
  {
    elle::String input =
      infinit::cryptography::random::generate<elle::String>(9128);
    elle::Buffer code = keypair.K().seal(input);
    elle::Buffer plain = keypair.k().open(code);
    elle::String const output(plain.string());

    BOOST_CHECK_EQUAL(input, output);
  }

  // Public/private encryption/decryption.
  {
    elle::String input = "a short string";
    elle::Buffer code = keypair.K().encrypt(input);
    elle::Buffer plain = keypair.k().decrypt(code);

    BOOST_CHECK_EQUAL(input, plain.string());
  }

  // Sign/verify a plain text.
  {
    elle::String input =
      infinit::cryptography::random::generate<elle::String>(1493);
    elle::Buffer signature = keypair.k().sign(input);
    auto result =
      keypair.K().verify(signature, input);

    BOOST_CHECK_EQUAL(result, true);
  }
}

static
void
test_operate()
{
  infinit::cryptography::rsa::KeyPair keypair =
    _test_generate(512,
                   infinit::cryptography::rsa::Padding::pkcs1,
                   infinit::cryptography::rsa::Padding::pss,
                   infinit::cryptography::Oneway::md5,
                   infinit::cryptography::Cipher::desx,
                   infinit::cryptography::Mode::cbc);

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
    infinit::cryptography::rsa::KeyPair keypair1 =
      _test_generate(1024,
                     infinit::cryptography::rsa::Padding::oaep,
                     infinit::cryptography::rsa::Padding::pkcs1,
                     infinit::cryptography::Oneway::sha512,
                     infinit::cryptography::Cipher::des2,
                     infinit::cryptography::Mode::cfb);

    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      keypair1.serialize(output);
    }

    typename elle::serialization::json::SerializerIn input(stream);
    infinit::cryptography::rsa::KeyPair keypair2(input);

    BOOST_CHECK_EQUAL(keypair1, keypair2);

    _test_operate(keypair2);
  }

  // Deserialize a wrong key pair (since missing elements)
  {
    elle::String representation(R"JSON({"private_key":"wrong key name", "public_key":"wrong key name again"}})JSON");
    std::stringstream stream(representation);
    typename elle::serialization::json::SerializerIn input(stream);

    BOOST_CHECK_THROW(infinit::cryptography::rsa::KeyPair keypair(input),
                      infinit::cryptography::Error);
  }

  // For every hard-coded strings [representation 1] in every format,
  // deserialize the key pairs, re-serialize them, make sure they can
  // be used and finally upgrade them: useful for detecting changes in formats.
  {
    std::vector<elle::String> const archives{
      // format 0
      R"JSON({"private key":{".version":{"major":0,"minor":0,"subminor":0},"digest algorithm":2,"encryption padding":3,"envelope cipher":5,"envelope mode":1,"rsa":"MIICXQIBAAKBgQDeiXQVuqQS02dwAXV3woFxqfl5NgXrE9TIv3IkDjL2DHCBU4VeXDvNVsgthJ0Cqcz5TOkO+bF5LANpeLAJ3+okB7mXmWsRJxwtFxYVZM1O85HRuqWJ1/0iucZhxI6ONHV0BDeAHqlQiSXfBOzORQc6nvcpYYtI2IOyK0CdpbZ4iwIDAQABAoGANewIfNtktksTXUh9Ni+jPe95y60Tcgq0HJCHD8WBA62v9SZPwrWGqBLHHoBrqyGJ0T6Zmk9WeY34pa3gugPPr8eFByugh2JMDSOG3erSQ6Wyc3Bzl2/ybVKONYlosiGLYrOXiuz0yvrruOlOt1QhDpjE0MyY7vpT1C2Y9D74vIECQQD50iRaO1yRoN6J1ZBCDaM5tP/I2P/kyTYUNu4kEqKxBEhzH5eevuXk8Pic29Dtvt7hnJ7uYS3Vbc32zfCwVirzAkEA5AqM5pc3srOstGS8kH82PMuFKYdJBb56sDUsUGyEC6vpKkUzGWQppYRpOij3ntpvLy0/f0JsNXdqLcmy5GayCQJAb+/F1BxnFOWE8TOdCMu9iFzeg2sf9a5mqdYXDFjBRxnJMLRGJp4YumVysC4aRnzQtzyLRfqLI+eoct7B6vEhGwJBANf2re4Ls/IHez30USg/cawtylTEDIHmHOcX1Hnt6zfqfQ1NL9GSLGbWeZldBvVoRPfW0FXDIBualfH7HPJ115ECQQCFYFKJb19q5kVElAEJaj/vj5RwAdNQKqfI0gWFIY3WGFz6WAEerAjQxzODlb55J+dZAUuqGd3y27WSViaY+AjM","signature padding":5},"public key":{".version":{"major":0,"minor":0,"subminor":0},"digest algorithm":2,"encryption padding":3,"envelope cipher":5,"envelope mode":1,"rsa":"MIGJAoGBAN6JdBW6pBLTZ3ABdXfCgXGp+Xk2BesT1Mi/ciQOMvYMcIFThV5cO81WyC2EnQKpzPlM6Q75sXksA2l4sAnf6iQHuZeZaxEnHC0XFhVkzU7zkdG6pYnX/SK5xmHEjo40dXQEN4AeqVCJJd8E7M5FBzqe9ylhi0jYg7IrQJ2ltniLAgMBAAE=","signature padding":5}})JSON"
      };

    infinit::cryptography::test::formats<infinit::cryptography::rsa::KeyPair>(
      archives, std::bind(_test_operate, std::placeholders::_1));
  }
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("rsa/KeyPair");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}
