#include "../cryptography.hh"

#include <cryptography/dsa/KeyPair.hh>
#include <cryptography/dsa/PublicKey.hh>
#include <cryptography/dsa/PrivateKey.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/random.hh>

#include <elle/printf.hh>
#include <elle/types.hh>
#include <elle/serialization/json.hh>

/*----------.
| Represent |
`----------*/

static
void
_test_represent(uint32_t const length,
                infinit::cryptography::Oneway const digest_algorithm)
{
  // 1)
  {
    infinit::cryptography::dsa::KeyPair keypair =
      infinit::cryptography::dsa::keypair::generate(length,
                                                    digest_algorithm);

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

  _test_represent(RUNNING_ON_VALGRIND ? 512 : 1024,
                  infinit::cryptography::Oneway::sha1);
}

/*---------.
| Generate |
`---------*/

static
infinit::cryptography::dsa::KeyPair
_test_generate(uint32_t const length,
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
  _test_generate(RUNNING_ON_VALGRIND ? 512 : 2048,
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
    _test_generate(RUNNING_ON_VALGRIND ? 512 : 2048,
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
    std::string input =
      infinit::cryptography::random::generate<std::string>(1493);
    elle::Buffer signature = keypair.k().sign(input);
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
                   infinit::cryptography::Oneway::sha1);

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
      _test_generate(RUNNING_ON_VALGRIND ? 512 : 1024,
                     infinit::cryptography::Oneway::sha512);

    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      keypair1.serialize(output);
    }

    typename elle::serialization::json::SerializerIn input(stream);
    infinit::cryptography::dsa::KeyPair keypair2(input);

    BOOST_CHECK_EQUAL(keypair1, keypair2);

    _test_operate(keypair2);
  }

  // For every hard-coded strings [representation 1] in every format,
  // deserialize the key pairs, re-serialize them, make sure they can
  // be used and finally upgrade them: useful for detecting changes in formats.
  {
    std::vector<std::string> const archives{
      // format 0
      R"JSON({"private key":{".version":"0.0.0","digest algorithm":2,"dsa":"MIIBvAIBAAKBgQDV69tk+qVpwiPeiBUVfesOjjGfBFdR8naWQKCYlkfn/yw/qnf0ay8HLB1MG6hoxo3Nh3jqJJpwEEAgTX6N/6nvkYnDQYsKXEu/xPoe9r0B6IJ8ZMT3n3rOtBJV0ttFzYWgDm3wE1M1Wy2Fy+qLUBlQIKFlnp/UWTh1TO1CubJ91QIVANymO3Ihghaz2O9w7LTfapBEL8oXAoGAPD05GSgCYNFyd7RcAGyOPYnTQlMnP67uUj4Yc8j+LYDwfN+YVXLyTe9cj8F9tWRS/n5P+wBygDxWGFq5Z4kLORnZAKmat82qLWNtkkB/FxGwXbWsC2dYY+e/37gWacMHWIwz+acCr7f2sfKw8LECBiMhhQnXg8XKNrH6oPIZOqMCgYEA0QHTFEkzlE5PQmnqG48KB43vFvsi2BdH11Ub4wFhGfZ0vt92EQjyv2dfXE6grPYqWJdxOaG+acr6h1wBemOMxieTwdJ1742wMk73OnMB6D0PKUvtIeyyetIc1p/htvVpBD6iX92WMsOwYq/FHrZr77bVQNVsqATOm0frsLhZeRMCFQDBVIxc53gmioEw4xz1bgO5iohUEQ=="},"public key":{".version":"0.0.0","digest algorithm":2,"dsa":"MIIBogKBgQDRAdMUSTOUTk9CaeobjwoHje8W+yLYF0fXVRvjAWEZ9nS+33YRCPK/Z19cTqCs9ipYl3E5ob5pyvqHXAF6Y4zGJ5PB0nXvjbAyTvc6cwHoPQ8pS+0h7LJ60hzWn+G29WkEPqJf3ZYyw7Bir8UetmvvttVA1WyoBM6bR+uwuFl5EwKBgQDV69tk+qVpwiPeiBUVfesOjjGfBFdR8naWQKCYlkfn/yw/qnf0ay8HLB1MG6hoxo3Nh3jqJJpwEEAgTX6N/6nvkYnDQYsKXEu/xPoe9r0B6IJ8ZMT3n3rOtBJV0ttFzYWgDm3wE1M1Wy2Fy+qLUBlQIKFlnp/UWTh1TO1CubJ91QIVANymO3Ihghaz2O9w7LTfapBEL8oXAoGAPD05GSgCYNFyd7RcAGyOPYnTQlMnP67uUj4Yc8j+LYDwfN+YVXLyTe9cj8F9tWRS/n5P+wBygDxWGFq5Z4kLORnZAKmat82qLWNtkkB/FxGwXbWsC2dYY+e/37gWacMHWIwz+acCr7f2sfKw8LECBiMhhQnXg8XKNrH6oPIZOqM="}})JSON"
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
