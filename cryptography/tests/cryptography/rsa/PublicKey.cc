#include "../cryptography.hh"
#include "../Sample.hh"

#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/keypair.hh>
#include <cryptography/Exception.hh>

#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

/*----------.
| Represent |
`----------*/

static elle::String const _input1("my balls are ok!");
static Sample const _input2(23293083121, "chodaboy");

static
void
_test_represent()
{
  infinit::cryptography::rsa::KeyPair keypair =
    infinit::cryptography::rsa::keypair::generate(2048);

  // 1)
  {
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << keypair.K();
    elle::printf("[representation 1] %s\n", archive);
  }

  // 2)
  {
    infinit::cryptography::Signature signature =
      keypair.k().sign(
        infinit::cryptography::Plain{_input1});
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << signature;
    elle::printf("[representation 2] %s\n", archive);
  }

  // 3)
  {
    infinit::cryptography::Signature signature = keypair.k().sign(_input2);
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << signature;
    elle::printf("[representation 3] %s\n", archive);
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
_test_generate(elle::Natural32 const length = 1024)
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
  elle::String representation("AAAAAA4BAAAAAAAAMIIBCgKCAQEAv8dH60kw7ZuZ28gf3p2wr5C7rElFRjFVC3RtrM6Ke8grEyZbVq07dgy4jgfWuD752Odde73/yYU5Z5h0++AocJb+0q5ZmLGclyslSHuXnoWg5Wwe2LwsXIQUy0sP86rJuX5wEAf3mz+OpwEUAbsTMm7jaI7bMM6y6qBW5Bqmamu1XxIwOp5MQnAiwn/uFwk7aJ/mI+NQ+orfIEPi5mczevfafziXb8qm5Nes36C0n0l0Bs88vnkwLZ3Nc5qYiqo0WbulqtIJOXP7kKUt+RbqONakxa3Zyr8iaCyj3vDbmPqW8KOEzgAS8/bJuhTPzXBZ+4EeJQK2xRafsViSyffGXwIDAQABAwAFAAQACgA=");

  auto extractor =
    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(representation);
  infinit::cryptography::rsa::PublicKey K(extractor);

  // The following operations are based on hard-coded base64 string which
  // represent the data on which to operate, in their serialized form.

  // Verify plain from [representation 2].
  {
    elle::String archive("AAAAAAABAAAAAAAAubQ1VXp6BARfgVKZW+ptXVRHiznuhxhFeH3TMdoxqLjqjN815oGD3QF6EHL+V11vwny4mJJ0zzJeFoEzqK3LzO3UYPEonWSsokn5mlix4t3AZM/Cbs9pJfB1LU9AYU25Z1aD4Kq2H71fdD1cdERlNJoMlb9lWAEHkhyik3YancjDCyQgZukDU4P76HaDXOnBh3p6cOiaHJ13kUsb+OspiclbFMQrRO5U9YPFrrwEfx6WXmWT4mPHiHefIJNLyIOtSiRd+vxPB2NKD724i1/WYs6kkc70ZE8yYe7kSPCKpGakAIkItafQ/v5gMoTD8BuEfzzQOOgV9ggPZTGMxIv5fA==");

    auto extractor =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive);
    infinit::cryptography::Signature signature(extractor);

    auto result =
      K.verify(signature,
               infinit::cryptography::Plain{_input1});

    BOOST_CHECK_EQUAL(result, true);
  }

  // Verify plain from [representation 3].
  {
    elle::String archive("AAAAAAABAAAAAAAAaBGtrirDjT3oAgw+ZmAg69SGLUiLTEvLb5lv5fWN+AjsP2MdPCl7zusOJEfeWAm4nmAWI9EvLIH1fn1S+EP3RgtLzkTpSKCG2qOkSAB49MiMlfq9EuGBYBILichZw0pUPvZmkHG/ewcQ0/IWMEEjUqqIlgNJ0mcaMh6pF91T+FLGvqbC9jJ9x+y7M1kz0oNw7CxiGg7hdfNosNcwNQclbIbDj1oEMjgc/T97AHiCgTcY8eECy31fxVQiRSQHgqE4NvBTgoaYex47xv0tYvF3WgC+RmeHyo4ZbKA6bqq0ygxOj5N+w/5OTokW72V4ZUt2i6tTppwtRUEFLs/3PTTXcA==");

    auto extractor =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive);
    infinit::cryptography::Signature signature(extractor);

    auto result = K.verify(signature, _input2);

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

    elle::String archive;
    elle::serialize::to_string(archive) << K1;

    auto extractor = elle::serialize::from_string(archive);
    infinit::cryptography::rsa::PublicKey K2(extractor);

    BOOST_CHECK_EQUAL(K1, K2);
  }

  // For every hard-coded strings [representation 1] in every format,
  // deserialize the public keys, re-serialize them, make sure they can
  // be used and finally upgrade them: useful for detecting changes in formats.
  {
    std::vector<elle::String> const archives{
      // format 0
      "AAAAAA4BAAAAAAAAMIIBCgKCAQEAv8dH60kw7ZuZ28gf3p2wr5C7rElFRjFVC3RtrM6Ke8grEyZbVq07dgy4jgfWuD752Odde73/yYU5Z5h0++AocJb+0q5ZmLGclyslSHuXnoWg5Wwe2LwsXIQUy0sP86rJuX5wEAf3mz+OpwEUAbsTMm7jaI7bMM6y6qBW5Bqmamu1XxIwOp5MQnAiwn/uFwk7aJ/mI+NQ+orfIEPi5mczevfafziXb8qm5Nes36C0n0l0Bs88vnkwLZ3Nc5qYiqo0WbulqtIJOXP7kKUt+RbqONakxa3Zyr8iaCyj3vDbmPqW8KOEzgAS8/bJuhTPzXBZ+4EeJQK2xRafsViSyffGXwIDAQABAwAFAAQACgA="
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
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("PublicyKey");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_compare));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}

