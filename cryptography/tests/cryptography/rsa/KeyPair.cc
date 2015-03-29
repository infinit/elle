#include "../cryptography.hh"
#include "../Sample.hh"

#include <cryptography/rsa/KeyPair.hh>
#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/PrivateKey.hh>
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
_test_represent()
{
  // 1)
  {
    infinit::cryptography::rsa::KeyPair keypair =
      infinit::cryptography::rsa::keypair::generate(1024);
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

  _test_represent();
}

/*---------.
| Generate |
`---------*/

static
infinit::cryptography::rsa::KeyPair
_test_generate(elle::Natural32 const length = 2048)
{
  infinit::cryptography::rsa::KeyPair keypair =
    infinit::cryptography::rsa::keypair::generate(length);

  return (keypair);
}

static
void
test_generate()
{
  _test_generate();
}

/*----------.
| Construct |
`----------*/

static
void
test_construct()
{
  infinit::cryptography::rsa::KeyPair keypair1 = _test_generate(1024);

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
  // Public/private encryption/decryption with plain.
  {
    elle::String input =
      infinit::cryptography::random::generate<elle::String>(9128);
    infinit::cryptography::Code code =
      keypair.K().encrypt(
        infinit::cryptography::Plain{input});
    infinit::cryptography::Clear clear = keypair.k().decrypt(code);
    elle::String const output(clear.buffer().string());

    BOOST_CHECK_EQUAL(input, output);
  }

  // Public/private encryption/decryption with complex type.
  {
    Sample const input(
      42, infinit::cryptography::random::generate<elle::String>(14920));
    infinit::cryptography::Code code = keypair.K().encrypt(input);
    Sample const output = keypair.k().decrypt<Sample>(code);

    BOOST_CHECK_EQUAL(input, output);
  }

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
  infinit::cryptography::rsa::KeyPair keypair = _test_generate(1024);

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
    infinit::cryptography::rsa::KeyPair keypair1 = _test_generate(512);

    elle::String archive;
    elle::serialize::to_string(archive) << keypair1;

    auto extractor = elle::serialize::from_string(archive);

    infinit::cryptography::rsa::KeyPair keypair2(extractor);

    BOOST_CHECK_EQUAL(keypair1, keypair2);

    _test_operate(keypair2);
  }

  // For every hard-coded strings [representation 1] in every format,
  // deserialize the key pairs, re-serialize them, make sure they can
  // be used and finally upgrade them: useful for detecting changes in formats.
  {
    std::vector<elle::String> const archives{
      // format 0
      "AAAAAAAAjAAAAAAAAAAwgYkCgYEAz+yogcqVOto2lrLcW7QIqJvsNim9PN9JClZ4aU9Y+OzOhZqQenibGuvzQGYunOW4W7NY4A9AEAOpPe9eYj9URbf3E1UBQ60p7ba4cNP4m+CKErh4EIc9vH6xSi+mK0jZweATr9z0IAGfZIm2NaqP+rXVtTnbf4tIsbMNkFeTtd0CAwEAAQMABQAEAAoAAAAAAGACAAAAAAAAMIICXAIBAAKBgQDP7KiBypU62jaWstxbtAiom+w2Kb0830kKVnhpT1j47M6FmpB6eJsa6/NAZi6c5bhbs1jgD0AQA6k9715iP1RFt/cTVQFDrSnttrhw0/ib4IoSuHgQhz28frFKL6YrSNnB4BOv3PQgAZ9kibY1qo/6tdW1Odt/i0ixsw2QV5O13QIDAQABAoGAKWI9F7gug4wzGUN4QHhQ0N5X4V/lXlxCuWAEj1FPsBvR7x2YbAfdNzqHK+YqmOfQDdBgfBgM9CMGaGFgM8XPC8Ulrm8eT1UhK2IP/A7lqFBxosZ7bzmL/1IoqMkhgJeLlyg0DnlsX3b/qi7QETId0sVuC2F/1dE31eZPB9nYAgECQQD7ro8BjGfSrIbsjJRpz1T9MVGbPNmBeb74p7f8MWcjVaECauX/7CrTlwmCrxVr6TxalsFBbviRIifFLqtdMn39AkEA033oUuRxVc8dFwnn5e15ArW3xRtjM2Qy8THxT+hyU0e0xUKaVvg9Vj5XwptrQFG0816nC8BNzlApuGQ6w+atYQJADnxRhslKAHolmd/NevmP7rKz+7dY3XbgYvdkQ4OkaAg5qCX27u/s1xjOV1M0bG2UJ7KdlOKKM/A/Ub4J5udNEQJBAJLjqg6Yl5bvqNatjKEHlLqrF54Q7Y9uEmuvMlk7qtvou8ceATBSbdeOqUmCxLUBitpVbVQeFmwrBS8Kfj4940ECQGd1wayrR0gr7QJbXgF4YKpY3qpRUzZSuBO5NXEdT16TIi+5RrXuwLZEuGUtqoIXJb3g+c3PLGjL592omGPnrVoDAAUABAA="
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
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("KeyPair");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}
