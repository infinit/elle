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
    elle::Buffer clear = keypair.k().open(code);
    elle::String const output(clear.string());

    BOOST_CHECK_EQUAL(input, output);
  }

  // Public/private encryption/decryption.
  {
    elle::String input = "a short string";
    elle::Buffer code = keypair.K().encrypt(input);
    elle::Buffer clear = keypair.k().decrypt(code);

    BOOST_CHECK_EQUAL(input, clear.string());
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
    elle::String representation(R"JSON({"private_key":{"algorithm":"rsa","modulus":"DAE3A85F69DB7C6BDADB252D4BC3025BB7BE26DAB340A3256BEAC35DBE09C0B0332010087B1435D8651B011F255D1A488A5B495B46B7BC2F7CF26F5853FAFBA8534634B839AEE3AACD546045D73A8EBDA1119F75B118764C1399ED201F2BB9410293F1440BC8E4BFDEDF176DA2EE0D91FC7C0A0BE16936F1F93034D7ABEDDEC9","private_exponent":"908F1D99B5FDA094527BBC77C7BB48896E3604CF16E72F57FD8793F30B18C1CB1C58A67E135AEB9CB859374A6C90AA55121AEBBD7E5CC5C9CF0905841E2279A41E7DB542A836D42DDA0E32820E7AA4034BF21ECC74885A2A557F9C0A8DE8E41C8F4557AEBB1D8051C6B292E434E873AFAC9DCD31041ECF0BA0A76E9E20EA8CA1","public_exponent":"010001"},"public_key":{"algorithm":"rsa","modulus":"DAE3A85F69DB7C6BDADB252D4BC3025BB7BE26DAB340A3256BEAC35DBE09C0B0332010087B1435D8651B011F255D1A488A5B495B46B7BC2F7CF26F5853FAFBA8534634B839AEE3AACD546045D73A8EBDA1119F75B118764C1399ED201F2BB9410293F1440BC8E4BFDEDF176DA2EE0D91FC7C0A0BE16936F1F93034D7ABEDDEC9","public_exponent":"010001"}})JSON");
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
      R"JSON({"private key":{"digest algorithm":2,"encryption padding":3,"rsa":"MIICWwIBAAKBgQC9Bmsc9SNwFy1d8f40Ol1p+pN/jt6Nok9o4wAHmdPQr5pnSRTiP/Hj5S71NtayBnA0kDK3EicS9Rbo3DHjY+XA+I2O/OSICiFdIAnVFMWxXmYMxalTWK/MYFqwLBF1mWsde8yemF4y6wx4sV9EBoJvPdnY+a7cNpd8biU8PUPkawIDAQABAoGAZrpzQhSTwtpFxJW8omPJ2svcl2ls2CqK36Tmojrem76julhEakV4T3OBlrDL84p+NoLbnRTXeX0Li1gIIN1swgjSl2fb0dR/J+hDotLs3cln2b+c0R6aw/D96ctNrtMlPUlwEOcdx7EIYjuG52hu7P3ZLlWJ5qYyNnDRtP7JZakCQQDj06Lnor+4ni7J76x+reMlyJdfgJvDf/Jb5w2bru3756PFNtGMVBWk+cwfAF0CrAlJ7ZeUe8U/omAFF89WnktnAkEA1GZt/PRCbt+wfkFaDetaOd5KGQZrf8wpxcM/BYvSS6Ov42lbk04J0XuRVrLNsNNkCuSkemTgcGKqqqyJxRmAXQJAajYrFyQXj2EnXPtAhppYcC/w6hQSCfVnLpMsE+1cg3i9L97bmDlHEe9myvknY2yCtBVw+kctcPopG5a7EZ2gdwJAeWArXFENIaTsi8m2t5uVzNntiA12Iuq9uEiR3A6B1z2sBo2viicfTzmI3X2CM5kpY1wzbcn1w+NuWegNNrgS9QJAJnujonVMplOdZbL0pKxeGKktrdmSJZ3zn4Gr/Tr6HjbRgvl+bB8J1yhmC3Vb3YEHVpweTi8YnZdwegLgA2dqWg==","signature padding":5,"version":{"major":0,"minor":0,"subminor":0}},"public key":{"digest algorithm":2,"encryption padding":3,"envelope cipher":5,"envelope mode":1,"rsa":"MIGJAoGBAL0Gaxz1I3AXLV3x/jQ6XWn6k3+O3o2iT2jjAAeZ09CvmmdJFOI/8ePlLvU21rIGcDSQMrcSJxL1FujcMeNj5cD4jY785IgKIV0gCdUUxbFeZgzFqVNYr8xgWrAsEXWZax17zJ6YXjLrDHixX0QGgm892dj5rtw2l3xuJTw9Q+RrAgMBAAE=","signature padding":5,"version":{"major":0,"minor":0,"subminor":0}}})JSON"
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
