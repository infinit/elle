#include "../cryptography.hh"

#include <cryptography/rsa/KeyPair.hh>
#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/Padding.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/Exception.hh>
#include <cryptography/random.hh>

#include <elle/printf.hh>
#include <elle/types.hh>
#include <elle/test.hh>
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
      typename elle::serialization::Json::SerializerOut output(stream);
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
  // Public/private encryption/decryption with plain.
  {
    printf("HERE\n");

    elle::String input =
      infinit::cryptography::random::generate<elle::String>(9128);
    infinit::cryptography::Code code =
      keypair.K().encrypt(infinit::cryptography::Plain(input));
    infinit::cryptography::Clear clear = keypair.k().decrypt(code);
    elle::String const output(clear.buffer().string());

    printf("HERE\n");

    BOOST_CHECK_EQUAL(input, output);
  }

  // Sign/verify a plain text.
  {
    elle::String input =
      infinit::cryptography::random::generate<elle::String>(1493);
    infinit::cryptography::Signature signature =
      keypair.k().sign(infinit::cryptography::Plain(input));
    auto result =
      keypair.K().verify(signature,
                         infinit::cryptography::Plain(input));

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
      typename elle::serialization::Json::SerializerOut output(stream);
      keypair1.serialize(output);
    }

    typename elle::serialization::Json::SerializerIn input(stream.str());
    infinit::cryptography::rsa::KeyPair keypair2(input);

    BOOST_CHECK_EQUAL(keypair1, keypair2);

    _test_operate(keypair2);
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
