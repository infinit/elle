#include "../cryptography.hh"

#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/KeyPair.hh>
#include <cryptography/Exception.hh>

#include <elle/printf.hh>
#include <elle/serialization/json.hh>

/*----------.
| Represent |
`----------*/

static elle::String const _input("chiche, donne nous tout!");

static
void
_test_represent()
{
  infinit::cryptography::rsa::KeyPair keypair =
    infinit::cryptography::rsa::keypair::generate(1024);

  // 1)
  {
    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      infinit::cryptography::rsa::PrivateKey k = keypair.k();
      k.serialize(output);
    }

    elle::printf("[representation 1] %s\n", stream.str());
  }

  // 2)
  {
    elle::Buffer code = keypair.K().seal(_input);

    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      output.serialize("code", code);
    }

    elle::printf("[representation 2] %s\n", stream.str());
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
infinit::cryptography::rsa::PrivateKey
_test_generate(elle::Natural32 const length = 1024)
{
  infinit::cryptography::rsa::KeyPair keypair =
    infinit::cryptography::rsa::keypair::generate(length);

  infinit::cryptography::rsa::PrivateKey k(keypair.k());

  return (k);
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
  infinit::cryptography::rsa::PrivateKey k1 = _test_generate(2048);

  // PrivateKey copy.
  infinit::cryptography::rsa::PrivateKey k2(k1);

  BOOST_CHECK_EQUAL(k1, k2);

  // PrivateKey move.
  infinit::cryptography::rsa::PrivateKey k3(std::move(k1));

  BOOST_CHECK_EQUAL(k2, k3);
}

/*--------.
| Operate |
`--------*/

static
void
test_operate()
{
  // Construct a private key from [representation 1]
  elle::String representation(R"JSON({"digest algorithm":4,"encryption padding":3,"rsa":"MIICXQIBAAKBgQCxSKncjZP3iCHw2ktyfAyWBWZVxnPyRdk6eLADFBdSvFVQxBAEyqOXhX/XLRMzJwz9hUT3fJu1oO2B7Rm9aFERfsvTES/XgMbi5VRGq1V+OFZFO3c2DD7iNqZWEibuBN4hyER469jA1GUWpgEF0X1x7Dgzqac0cH6kUUdAm5KG5QIDAQABAoGADbkTNPhcmM+X2P380xrZGo/GcSNQfFN90jHfZg2bFJNxgzP6DXTmHud9ktlxYAdI0SMMyJ+DqhaP6/u/f1Lg23mygS8P1XdgjcC6FYGXxMwQZkaNnOZawmZ2+w7M+at4l6yDmQsWlvbC/EsKO5cLgfFa3RvUmBocSIjAJ8JIM0ECQQDq7TwJOVzgbcMzEasL3DGJopwm7aXGM35poYApLvQEPiCXnRuU5MzVki8PgBp/qoeouR0DsSdcXkNJ3KVAy1IVAkEAwS+9qEibQ/Lt3/z09YzcDpJwQtFZkrcg7P5MqTtfKKEOm59DKkLTE11sZ+ZEoCNNu8+A44qRW5AfX+TnblwlkQJBALnp9FD4bF/PfWK04MAKCcn6Pckw2Lf5mFjPzjTJS6Lv56sUUfeK+e8qOMPzhQTzV1hW5nc9gwC+OFIDo2Obg+ECQFrzJPEDLcXxqrfXGWQ5KqjZnGx4rxj5ilDI8EKEJyqzmHL/qdlNmJ33T8AikpBpzoDrEh0N8Z0P3xWAaMx2KWECQQCdlC2RVeemFXgE6YCB4oWlt9sHd3Rns2upFI4vCBaEeT5FZhCn5AIm5f65AioGlCAUOPqkRvpZVOzYaQVic5fm","signature padding":5})JSON");

  std::stringstream stream(representation);
  typename elle::serialization::json::SerializerIn input(stream);
  infinit::cryptography::rsa::PrivateKey k(input);

  // The following operations are based on hard-coded base64 string which
  // represent the data on which to operate, in their serialized form.

  // Decrypt plain from [representation 2]
  {
    elle::String archive(R"JSON({"code":"QIBrD4Vau4w2OMzScnQXTJ2J807E4Wa52PAbEQEQfnK9dqC4GipZ8Ust+hzLx4RZPuEsupdUldSYA5AqZn2W4g1TFipDuC30ecXNYW6DEVwdKsqPsBTpsOOwu8BDsnagoU/g5G8rFDqop9fB5liJU2RR0s0fV/DAwcfUKzlwVLC9HTBTYWx0ZWRfXxP/oCAZe8E9a0zF6qExi9ZfcrPdGHOtujp1D4Nejd9ZvnZCKun/U/M="})JSON");

    std::stringstream stream(archive);
    typename elle::serialization::json::SerializerIn input(stream);
    elle::Buffer code;
    input.serialize("code", code);

    elle::Buffer clear = k.open(code);

    BOOST_CHECK_EQUAL(_input, clear.string());
  }
}

/*--------.
| Compare |
`--------*/

static
void
test_compare()
{
  infinit::cryptography::rsa::PrivateKey k1 = _test_generate(1024);
  infinit::cryptography::rsa::PrivateKey k2 = _test_generate(1024);

  // With high probabilituy, this should not be the case. Otherwise,
  // the random generator is probably broken.
  BOOST_CHECK(k1 != k2);
  BOOST_CHECK(!(k1 == k2));
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
    infinit::cryptography::rsa::PrivateKey k1 = _test_generate(2048);

    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      k1.serialize(output);
    }

    typename elle::serialization::json::SerializerIn input(stream);
    infinit::cryptography::rsa::PrivateKey k2(input);

    BOOST_CHECK_EQUAL(k1, k2);
  }

  // For every hard-coded strings [representation 1] in every format,
  // deserialize the public keys, re-serialize them, make sure they can
  // be used and finally upgrade them: useful for detecting changes in formats.
  {
    std::vector<elle::String> const archives{
      // format 0
      R"JSON({"digest algorithm":4,"encryption padding":3,"rsa":"MIICXQIBAAKBgQCxSKncjZP3iCHw2ktyfAyWBWZVxnPyRdk6eLADFBdSvFVQxBAEyqOXhX/XLRMzJwz9hUT3fJu1oO2B7Rm9aFERfsvTES/XgMbi5VRGq1V+OFZFO3c2DD7iNqZWEibuBN4hyER469jA1GUWpgEF0X1x7Dgzqac0cH6kUUdAm5KG5QIDAQABAoGADbkTNPhcmM+X2P380xrZGo/GcSNQfFN90jHfZg2bFJNxgzP6DXTmHud9ktlxYAdI0SMMyJ+DqhaP6/u/f1Lg23mygS8P1XdgjcC6FYGXxMwQZkaNnOZawmZ2+w7M+at4l6yDmQsWlvbC/EsKO5cLgfFa3RvUmBocSIjAJ8JIM0ECQQDq7TwJOVzgbcMzEasL3DGJopwm7aXGM35poYApLvQEPiCXnRuU5MzVki8PgBp/qoeouR0DsSdcXkNJ3KVAy1IVAkEAwS+9qEibQ/Lt3/z09YzcDpJwQtFZkrcg7P5MqTtfKKEOm59DKkLTE11sZ+ZEoCNNu8+A44qRW5AfX+TnblwlkQJBALnp9FD4bF/PfWK04MAKCcn6Pckw2Lf5mFjPzjTJS6Lv56sUUfeK+e8qOMPzhQTzV1hW5nc9gwC+OFIDo2Obg+ECQFrzJPEDLcXxqrfXGWQ5KqjZnGx4rxj5ilDI8EKEJyqzmHL/qdlNmJ33T8AikpBpzoDrEh0N8Z0P3xWAaMx2KWECQQCdlC2RVeemFXgE6YCB4oWlt9sHd3Rns2upFI4vCBaEeT5FZhCn5AIm5f65AioGlCAUOPqkRvpZVOzYaQVic5fm","signature padding":5})JSON"
      };

    infinit::cryptography::test::formats<
      infinit::cryptography::rsa::PrivateKey>(archives);
  }
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("rsa/PrivateyKey");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_compare));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}
