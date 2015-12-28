#include "../cryptography.hh"

#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/KeyPair.hh>

#include <elle/serialization/json.hh>

/*----------.
| Represent |
`----------*/

static elle::Buffer const _input("my balls are ok!");

static
void
_test_represent()
{
  infinit::cryptography::rsa::KeyPair keypair =
    infinit::cryptography::rsa::keypair::generate(2048);

  // 1)
  {
    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      infinit::cryptography::rsa::PublicKey K = keypair.K();
      K.serialize(output);
    }

    elle::printf("[representation 1] %s\n", stream.str());
  }

  // 2)
  {
    elle::Buffer signature = keypair.k().sign(_input);

    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      output.serialize("signature", signature);
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
infinit::cryptography::rsa::PublicKey
_test_generate(uint32_t const length = 1024)
{
  infinit::cryptography::rsa::KeyPair keypair =
    infinit::cryptography::rsa::keypair::generate(length);

  infinit::cryptography::rsa::PublicKey K(keypair.K());

  return (K);
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
  infinit::cryptography::rsa::PublicKey K1 = _test_generate(2048);

  // PublicKey copy.
  infinit::cryptography::rsa::PublicKey K2(K1);

  BOOST_CHECK_EQUAL(K1, K2);

  // PublicKey move.
  infinit::cryptography::rsa::PublicKey K3(std::move(K1));

  BOOST_CHECK_EQUAL(K2, K3);
}

/*--------.
| Operate |
`--------*/

static
void
test_operate()
{
  // Construct a public key from [representation 1]
  std::string representation(R"JSON({"rsa":"MIIBCgKCAQEAplfeEnFRCbdId8WiyP89bsoLSfE6kRDV4Vbu/gADaVv/sDitbLkOJGyzpIKP/yjmNqvj/m5mfKrHGqbe23c0g8qoTsR1tQN4xHfDm/1n05JyHL7f2Ij/JzaYR87aErYhAKuVM03YANLpV/ykXFBiLZ5NnY06v3aLWwKzJKSrmxB2t0tG0xnLQmvfxAFxhDH3+hUoXYXa9kTsJ0uuBBQfw2cyinRNs+nF7jerySK1RYe+5qmOfaeNlVZiD/kvXMgjGG3KL7kAw7xqhaRZrDDinTK+QwyDgCKUKIRir98QaTOAbveip4a5jkFljk58mCSO4QieM5q69pS79OXoZUOAJwIDAQAB"})JSON");

  std::stringstream stream(representation);
  typename elle::serialization::json::SerializerIn input(stream);
  infinit::cryptography::rsa::PublicKey K(input);

  // The following operations are based on hard-coded base64 string which
  // represent the data on which to operate, in their serialized form.

  // Verify plain from [representation 2].
  {
    std::string archive(R"JSON({"signature":"d483mVIqqV5b/UGAP54stJrV9m55eAurgxAvh7OGfNv8XG4QnAqmjOPgLa/H1vOMAWTxd/HHCP4sLTKYmRhHwLXPKFDGrBhyVBMh/PgVsiF0keHFLbmpTP42PEWF4H/xt8Z3NeOH87F5RPdyqaAhW8LcA4MNILo+S6KPJPvw9jtH3seNsTJfHaoGl0n3uVJS1EMbEiQB8Cu+GGnLe3DGgg/4YV2vFiM/v0Cfc1UKWmfYlnaGfb3hbq2KunMRLb5t2WgjM6qyZKtdSKhIDEenr279hD8JrD89iVw4I7ZV64pw9SG1B37ms0IqkY1zG51MMv9SPNNYtuSFOix4w2IpHg=="})JSON");

    std::stringstream stream(archive);
    typename elle::serialization::json::SerializerIn input(stream);
    elle::Buffer signature;
    input.serialize("signature", signature);

    auto result = K.verify(signature, _input);

    BOOST_CHECK_EQUAL(result, true);
  }
}

/*--------.
| Compare |
`--------*/

static
void
test_compare()
{
  infinit::cryptography::rsa::PublicKey K1 = _test_generate(1024);
  infinit::cryptography::rsa::PublicKey K2 = _test_generate(1024);

  // With high probability, this should not be the case. Otherwise,
  // the random generator is probably broken.
  BOOST_CHECK(K1 != K2);
  BOOST_CHECK(!(K1 == K2));
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
    infinit::cryptography::rsa::PublicKey K1 = _test_generate(2048);

    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      K1.serialize(output);
    }

    typename elle::serialization::json::SerializerIn input(stream);
    infinit::cryptography::rsa::PublicKey K2(input);

    BOOST_CHECK_EQUAL(K1, K2);
  }

  // For every hard-coded strings [representation 1] in every format,
  // deserialize the public keys, re-serialize them, make sure they can
  // be used and finally upgrade them: useful for detecting changes in formats.
  {
    std::vector<std::string> const archives{
      // format 0
      R"JSON({"rsa":"MIIBCgKCAQEAplfeEnFRCbdId8WiyP89bsoLSfE6kRDV4Vbu/gADaVv/sDitbLkOJGyzpIKP/yjmNqvj/m5mfKrHGqbe23c0g8qoTsR1tQN4xHfDm/1n05JyHL7f2Ij/JzaYR87aErYhAKuVM03YANLpV/ykXFBiLZ5NnY06v3aLWwKzJKSrmxB2t0tG0xnLQmvfxAFxhDH3+hUoXYXa9kTsJ0uuBBQfw2cyinRNs+nF7jerySK1RYe+5qmOfaeNlVZiD/kvXMgjGG3KL7kAw7xqhaRZrDDinTK+QwyDgCKUKIRir98QaTOAbveip4a5jkFljk58mCSO4QieM5q69pS79OXoZUOAJwIDAQAB"})JSON"
      };

    infinit::cryptography::test::formats<
      infinit::cryptography::rsa::PublicKey>(archives);
  }
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("rsa/PublicyKey");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_compare));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}
