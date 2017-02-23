#include "../cryptography.hh"

#include <elle/cryptography/rsa/KeyPair.hh>
#include <elle/cryptography/rsa/PublicKey.hh>
#include <elle/cryptography/rsa/PrivateKey.hh>
#include <elle/cryptography/rsa/Padding.hh>
#include <elle/cryptography/Oneway.hh>
#include <elle/cryptography/Cipher.hh>
#include <elle/cryptography/Error.hh>
#include <elle/cryptography/random.hh>

#include <elle/printf.hh>
#include <elle/types.hh>
#include <elle/serialization/json.hh>

ELLE_LOG_COMPONENT("elle.cryptography.test");

/*----------.
| Represent |
`----------*/

static
void
_test_represent(uint32_t const length)
{
  // 1)
  {
    elle::cryptography::rsa::KeyPair keypair =
      elle::cryptography::rsa::keypair::generate(length);

    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      keypair.serialize(output);
    }

    elle::fprintf(std::cout, "[representation 1] %s\n", stream.str());
  }
}

static
void
represent()
{
  // WARNING: To uncomment only if one wants to update the representations.
  return;

  // These generate base64-based representations which can be used in
  // other tests.

  _test_represent(1024);
}

/*---------.
| Generate |
`---------*/

static
elle::cryptography::rsa::KeyPair
_test_generate(uint32_t const length)
{
  elle::cryptography::rsa::KeyPair keypair =
    elle::cryptography::rsa::keypair::generate(length);

  return (keypair);
}

static
void
generate()
{
  _test_generate(4096);
}

/*----------.
| Construct |
`----------*/

static
void
construct()
{
  elle::cryptography::rsa::KeyPair keypair1 = _test_generate(2048);

  // KeyPair copy.
  elle::cryptography::rsa::KeyPair keypair2(keypair1);

  BOOST_CHECK_EQUAL(keypair1, keypair2);

  // KeyPair move.
  elle::cryptography::rsa::KeyPair keypair3(std::move(keypair1));

  BOOST_CHECK_EQUAL(keypair2, keypair3);

  // Attributes copy.
  elle::cryptography::rsa::KeyPair keypair4(keypair2.K(), keypair2.k());

  BOOST_CHECK_EQUAL(keypair2, keypair3);
  BOOST_CHECK_EQUAL(keypair2, keypair4);
  BOOST_CHECK_EQUAL(keypair3, keypair4);

  // Attributes move.
  elle::cryptography::rsa::PublicKey K(keypair3.K());
  elle::cryptography::rsa::PrivateKey k(keypair3.k());

  elle::cryptography::rsa::KeyPair keypair5(std::move(K), std::move(k));

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
_test_operate(elle::cryptography::rsa::KeyPair const& keypair)
{
  // Public/private seal/open.
  {
    auto input = elle::cryptography::random::generate<elle::Buffer>(9128);
    elle::Buffer code = keypair.K().seal(input);
    elle::Buffer plain = keypair.k().open(code);
    std::string const output(plain.string());

    BOOST_CHECK_EQUAL(input, output);
  }

  // Public/private encryption/decryption.
  {
    std::string input = "a short string";
    elle::Buffer code = keypair.K().encrypt(input);
    elle::Buffer plain = keypair.k().decrypt(code);

    BOOST_CHECK_EQUAL(input, plain.string());
  }

  // Sign/verify a plain text.
  {
    auto input = elle::cryptography::random::generate<elle::Buffer>(1493);
    elle::Buffer signature = keypair.k().sign(input);
    auto result =
      keypair.K().verify(signature, input);

    BOOST_CHECK_EQUAL(result, true);
  }
}

static
void
operate()
{
  elle::cryptography::rsa::KeyPair keypair = _test_generate(512);

  _test_operate(keypair);
}

/*----------.
| Serialize |
`----------*/

static
void
serialize()
{
  // Serialize/deserialize.
  {
    elle::cryptography::rsa::KeyPair keypair1 = _test_generate(1024);

    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      keypair1.serialize(output);
    }

    typename elle::serialization::json::SerializerIn input(stream);
    elle::cryptography::rsa::KeyPair keypair2(input);

    BOOST_CHECK_EQUAL(keypair1, keypair2);

    _test_operate(keypair2);
  }

  // Deserialize a wrong key pair (since missing elements).
  {
    std::string representation(R"JSON({"private_key":"wrong key name", "public_key":"wrong key name again"}})JSON");
    std::stringstream stream(representation);
    typename elle::serialization::json::SerializerIn input(stream);

    BOOST_CHECK_THROW(elle::cryptography::rsa::KeyPair keypair(input),
                      elle::cryptography::Error);
  }

  // For every hard-coded strings [representation 1] in every format,
  // deserialize the key pairs, re-serialize them, make sure they can
  // be used and finally upgrade them: useful for detecting changes in formats.
  {
    std::vector<std::string> const archives{
      // format 0
      R"JSON({"private key":{".version":"0.0.0","rsa":"MIICXQIBAAKBgQDeiXQVuqQS02dwAXV3woFxqfl5NgXrE9TIv3IkDjL2DHCBU4VeXDvNVsgthJ0Cqcz5TOkO+bF5LANpeLAJ3+okB7mXmWsRJxwtFxYVZM1O85HRuqWJ1/0iucZhxI6ONHV0BDeAHqlQiSXfBOzORQc6nvcpYYtI2IOyK0CdpbZ4iwIDAQABAoGANewIfNtktksTXUh9Ni+jPe95y60Tcgq0HJCHD8WBA62v9SZPwrWGqBLHHoBrqyGJ0T6Zmk9WeY34pa3gugPPr8eFByugh2JMDSOG3erSQ6Wyc3Bzl2/ybVKONYlosiGLYrOXiuz0yvrruOlOt1QhDpjE0MyY7vpT1C2Y9D74vIECQQD50iRaO1yRoN6J1ZBCDaM5tP/I2P/kyTYUNu4kEqKxBEhzH5eevuXk8Pic29Dtvt7hnJ7uYS3Vbc32zfCwVirzAkEA5AqM5pc3srOstGS8kH82PMuFKYdJBb56sDUsUGyEC6vpKkUzGWQppYRpOij3ntpvLy0/f0JsNXdqLcmy5GayCQJAb+/F1BxnFOWE8TOdCMu9iFzeg2sf9a5mqdYXDFjBRxnJMLRGJp4YumVysC4aRnzQtzyLRfqLI+eoct7B6vEhGwJBANf2re4Ls/IHez30USg/cawtylTEDIHmHOcX1Hnt6zfqfQ1NL9GSLGbWeZldBvVoRPfW0FXDIBualfH7HPJ115ECQQCFYFKJb19q5kVElAEJaj/vj5RwAdNQKqfI0gWFIY3WGFz6WAEerAjQxzODlb55J+dZAUuqGd3y27WSViaY+AjM"},"public key":{".version":"0.0.0","rsa":"MIGJAoGBAN6JdBW6pBLTZ3ABdXfCgXGp+Xk2BesT1Mi/ciQOMvYMcIFThV5cO81WyC2EnQKpzPlM6Q75sXksA2l4sAnf6iQHuZeZaxEnHC0XFhVkzU7zkdG6pYnX/SK5xmHEjo40dXQEN4AeqVCJJd8E7M5FBzqe9ylhi0jYg7IrQJ2ltniLAgMBAAE="}})JSON"
      };

    elle::cryptography::test::formats<elle::cryptography::rsa::KeyPair>(
      archives, std::bind(_test_operate, std::placeholders::_1));
  }
}

/*--------.
| Signing |
`--------*/

class Signed
{
public:
  Signed(int i, int j)
    : _i(i)
    , _j(j)
  {}

  void
  serialize(elle::serialization::Serializer& s, elle::Version const& v)
  {
    s.serialize("i", this->_i);
    if (v >= elle::Version(0, 1, 0))
      s.serialize("j", this->_j);
  }

  ELLE_ATTRIBUTE_R(int, i);
  ELLE_ATTRIBUTE_R(int, j);

  struct serialization_tag {
    static elle::Version version;
  };
};

elle::Version Signed::serialization_tag::version{0,1,0};

static
void
signing()
{
  Signed s(1, 2);
  Signed s2(1, 3);
  elle::cryptography::rsa::KeyPair keys =
    elle::cryptography::rsa::keypair::generate(1024);
  ELLE_LOG("sign with legacy version")
  {
    auto signature = keys.k().sign(s, elle::Version(0, 0, 0));
    ELLE_DUMP("signature: %s", signature);
    BOOST_CHECK(keys.K().verify(signature, s));
    BOOST_CHECK(keys.K().verify(signature, s2));
  }
  ELLE_LOG("sign with new version")
  {
    auto signature = keys.k().sign(s, elle::Version(0, 1, 0));
    ELLE_DUMP("signature: %s", signature);
    BOOST_CHECK(keys.K().verify(signature, s));
    BOOST_CHECK(!keys.K().verify(signature, s2));
  }
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(represent));
  suite.add(BOOST_TEST_CASE(generate));
  suite.add(BOOST_TEST_CASE(construct));
  suite.add(BOOST_TEST_CASE(operate));
  suite.add(BOOST_TEST_CASE(serialize));
  suite.add(BOOST_TEST_CASE(signing));
}
