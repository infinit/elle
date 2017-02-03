#include "../cryptography.hh"

#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/KeyPair.hh>

#include <elle/printf.hh>
#include <elle/serialization/json.hh>

/*----------.
| Represent |
`----------*/

static std::string const _input("chiche, donne nous tout!");

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

    elle::fprintf(std::cout, "[representation 1] %s\n", stream.str());
  }

  // 2)
  {
    elle::Buffer code = keypair.K().seal(_input);

    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      output.serialize("code", code);
    }

    elle::fprintf(std::cout, "[representation 2] %s\n", stream.str());
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
_test_generate(uint32_t const length = 1024)
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
  std::string representation(R"JSON({"rsa":"MIICXgIBAAKBgQDzicS9Tn1LqDpFtUC/2tEQFjuVmafURevYoyZS8A/MQ8LCN3k5rvb81XT/ofMzANCeiPISy/YcaetNMpxDZdYYTtKXnX/BCfLdIe1L3ljfT7nbYjL/jhjv4957QGEG1L5NlJVeowW2IYplis2VSKEUAomm39MU77lLKVn5gnmQyQIDAQABAoGBAKZf5Av/5xEcqFKe9NEcx95ZkfMtub4G1ox1SOU4I1LEp/TOuP5zTUCFAxzwopihYeQsYlP9CvF2Ws2uWZqho5Q6e7CcEFKzeX3q9qKd5Vu6kA0l5mvtHSWZj7GWEJijXbbUqHI3l8ENMOENDtjvzZtPGM+qiSN0t/tzas9E5CVBAkEA+gE1QvuEKTH+AOa67Bj++EtkrbKtj7StaIN+xEmNK3NdOq2Fbo90Dc4cCRlxhTPXvIxjWm1Slx1jpQ15nl4JdQJBAPlg3KYIX4eCCSoCzmYX7vrNrdwd9nEnhUA/i6CDb5NBC7zUKZ7k88KH5hLM93gYtSr0X7PSaPQ04/PIvqboK4UCQQDBbcHZy2Qbw3Q1Gk+xcD2Fuj9xLPegYjhwzpx8HjQ9VArj5XGBczhlnfyPBcKRPuVCGpScWE04YiLAh0Z7w2IpAkEAl5zQCRFmIthoAeaa3W8Hl+ucHAsLXVgNpzX7CadfGkOdgzvp4fSITmiY9C0w9w7udiMEXCs8K9xMClatsoPMkQJAUI2bk+0A2VtwGssgYm8EmTAEvD0BlPL2pIMbV5Frr2O2XCh5DH7iOcvEYJYGfVX8nK/HJdQ9ltgf86rRrLY8jA=="})JSON");

  std::stringstream stream(representation);
  typename elle::serialization::json::SerializerIn input(stream);
  infinit::cryptography::rsa::PrivateKey k(input);

  // The following operations are based on hard-coded base64 string which
  // represent the data on which to operate, in their serialized form.

  // Decrypt plain from [representation 2]
  {
    std::string archive(R"JSON({"code":"Lt2x0nEDpsdo7sUU4dI4tGjvfkp0pD8yGDfhTJ/UmtmPMXpisDKCjTBjCvHZqLf7JXH2mkzVI94AAVU9VRJksyvhNMcMr4yXSwCkpnhq2UjILhZsyP43CStDg2Ib624CA2ojSjiKzbrD5OvFyGnJ72pEmUMFuPAjC0+I6hEBMnNh4WKkbec3p0gOqSNAWEGIzK11VIOVq26b4n2s1vcXDhNGy8qpSWdvEtaq42EPjvE="})JSON");

    std::stringstream stream(archive);
    typename elle::serialization::json::SerializerIn input(stream);
    elle::Buffer code;
    input.serialize("code", code);

    elle::Buffer plain = k.open(code);

    BOOST_CHECK_EQUAL(_input, plain.string());
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
    std::vector<std::string> const archives{
      // format 0
      R"JSON({"rsa":"MIICXgIBAAKBgQDzicS9Tn1LqDpFtUC/2tEQFjuVmafURevYoyZS8A/MQ8LCN3k5rvb81XT/ofMzANCeiPISy/YcaetNMpxDZdYYTtKXnX/BCfLdIe1L3ljfT7nbYjL/jhjv4957QGEG1L5NlJVeowW2IYplis2VSKEUAomm39MU77lLKVn5gnmQyQIDAQABAoGBAKZf5Av/5xEcqFKe9NEcx95ZkfMtub4G1ox1SOU4I1LEp/TOuP5zTUCFAxzwopihYeQsYlP9CvF2Ws2uWZqho5Q6e7CcEFKzeX3q9qKd5Vu6kA0l5mvtHSWZj7GWEJijXbbUqHI3l8ENMOENDtjvzZtPGM+qiSN0t/tzas9E5CVBAkEA+gE1QvuEKTH+AOa67Bj++EtkrbKtj7StaIN+xEmNK3NdOq2Fbo90Dc4cCRlxhTPXvIxjWm1Slx1jpQ15nl4JdQJBAPlg3KYIX4eCCSoCzmYX7vrNrdwd9nEnhUA/i6CDb5NBC7zUKZ7k88KH5hLM93gYtSr0X7PSaPQ04/PIvqboK4UCQQDBbcHZy2Qbw3Q1Gk+xcD2Fuj9xLPegYjhwzpx8HjQ9VArj5XGBczhlnfyPBcKRPuVCGpScWE04YiLAh0Z7w2IpAkEAl5zQCRFmIthoAeaa3W8Hl+ucHAsLXVgNpzX7CadfGkOdgzvp4fSITmiY9C0w9w7udiMEXCs8K9xMClatsoPMkQJAUI2bk+0A2VtwGssgYm8EmTAEvD0BlPL2pIMbV5Frr2O2XCh5DH7iOcvEYJYGfVX8nK/HJdQ9ltgf86rRrLY8jA=="})JSON"
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
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("rsa/PrivateKey");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_compare));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}
