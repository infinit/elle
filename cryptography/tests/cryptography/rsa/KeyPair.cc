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
      "AAAAAAAAjAAAAAAAAAAwgYkCgYEAp94HNg5qw/G8LS837d02wENBh6sg4evdzw/HRYDtI8/l0IoY5vdWcD0pwDDBw1G7lbg6pkLMa0DF3Vem2c5tbk+b/A8v3rlDJ6/Z9l3vsODeGzRxXQIyB+K2ww0LQ9X5Wt429pz4hMi8Jx9hFiqfOp/oxVunx7qryEmHXDuQ0/8CAwEAAQAAAABgAgAAAAAAADCCAlwCAQACgYEAp94HNg5qw/G8LS837d02wENBh6sg4evdzw/HRYDtI8/l0IoY5vdWcD0pwDDBw1G7lbg6pkLMa0DF3Vem2c5tbk+b/A8v3rlDJ6/Z9l3vsODeGzRxXQIyB+K2ww0LQ9X5Wt429pz4hMi8Jx9hFiqfOp/oxVunx7qryEmHXDuQ0/8CAwEAAQKBgQChlOMmEkyMnaQ/bgMM7/Eisdpi58M4E9ym0g5DBa292gDhcfw0XgIS+Zy1hXkNNCA3JmU14nh4CJosy5EseDn183tOeIUkwl+amkrJFS5HE4hrBQ9AC1GF6rQ9DI+2VJt1uFSqNhrlVamkPprTyRBATdeTjIk7bM8cRdQ+T/Dr+QJBAN8dLi26u3bx6X34WjfnZi4fwrH+oiZF1N9OaQK+lZNO3+3+MvDmdPFl/XOt0LB3Glo5RU2HQkOC7WqAaA/kGeMCQQDAnDWxXKuHjD+cj2xBJLW+p//VRQ8i5sCVuTL4Hrj/OXaupg5eCC/GyJMPS4TuR8pP67FvKpLTII5EDXdvRCg1AkBQ6K4UW0EcM0HvMp7U4w9E7rOWBLcclfpKOI8QNbjcQKAoGCS1A32pBMlSp9ekBq0Bxv8AzGoVPWeHVaVnDdrtAkBuVY2UakeAQGD4KFiVAJhz/PzaiNLg5VqQ6iQ+aisDWUGZTlqQlxP3RVKowlQFw24rVhKwYaz18IJOjYBLZrm5AkBaOB8zMs9mdqn91DVwjUvV31XlS7CvfH2IPWwSiXqtWh3jWh2bbAvMUKh/DK1peZ/cdscyQQI07SyWnOF8ql4I"
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
