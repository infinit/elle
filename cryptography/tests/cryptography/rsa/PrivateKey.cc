#include "../cryptography.hh"
#include "../Sample.hh"

#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/KeyPair.hh>
#include <cryptography/Exception.hh>

#include <elle/printf.hh>
#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

/*----------.
| Represent |
`----------*/

static elle::String const _input1("chiche, donne nous tout!");
static Sample const _input2(3240913, "Dave");

static
void
_test_represent()
{
  infinit::cryptography::rsa::KeyPair keypair =
    infinit::cryptography::rsa::keypair::generate(1024);

  // 1)
  {
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << keypair.k();
    elle::printf("[representation 1] %s\n", archive);
  }

  // 2)
  {
    infinit::cryptography::Code code =
      keypair.K().encrypt(
        infinit::cryptography::Plain{_input1});
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << code;
    elle::printf("[representation 2] %s\n", archive);
  }

  // 3)
  {
    infinit::cryptography::Code code = keypair.K().encrypt(_input2);
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << code;
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
  elle::String representation("AgAAAGACAAAAAAAAMIICXAIBAAKBgQDYvPRs+PsQRLs4+n9JXsRN2FWOetpxjmRikrgaNbnIcqoxpLrVq0sjDna3KNiHOmQI1bprBhoAGnDDspdd0NOqinkzJCfyjIKmiQtmQ/XOMcSy5weUjtvST9u06i6Pq/9df0lAicTcxmrXGX/gXpq5OY8SyWnM+0tEw2044d+YwwIDAQABAoGBALHzyrhiYTkFRbdNrQ7fuUu1EOIwKoTMBc4Naw1SCVz0EJs8Bo8C2mlJKd3OsqGtJo8+dJYE53ooO2fijXdgyzgaTGHMeAQJxNKv4lJdP7sfCPS7gSTn5Ob39Ap3sgCe/K0Jy5Y/URqEV95sIOszgQ5TfMSAUIoW1Pv0C5mGqaBpAkEA/Ei/qP+FdavFKhTp39LgldGhikEu6qxQWP2dUZE2w154U+UIV2hK70fR2UTgTCFkErri/+gKByLJO3S8ps9yLwJBANvuLX5Cv1HvDSf/pJ5SMCZPCGC/hnCZpRmDGgdNTykWXnOy4/WtWWdq8RkM3oxfA7piZ5t3uwy2KzCfsmKGwa0CQBakiN8oH9hR1F5UhjEagiGwMOlxtD5HvOZfzNDXUazzO6V79uWxbJhRP9paPi9XB95Oe7UwJEZDrv/xGEV4dO0CQClcSJf3K43YHCxKioXvxPhfYp73RexFKOR8YFfBNULv3wF5Qh/A4xBs7zyby/qwH75cpUl6oeXJGKRDVEPtPd0CQGF2zpA4h1RnEMdkl6RL433zP49vqjzZDACaonI1SxSyi7OXcFRAbM30+I2wjKwqOAf1haAd2hB0Se/C25GcKXsDAAUABAA=");
  auto extractor =
    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(representation);
  infinit::cryptography::rsa::PrivateKey k(extractor);

  // The following operations are based on hard-coded base64 string which
  // represent the data on which to operate, in their serialized form.

  // Decrypt plain from [representation 2]
  {
    elle::String archive("AAAAAMgAAAAAAAAAAAAAAIAAAAAAAAAAjSbtl+OyMhvurakrVUUP+Q1w1HxlIqm7YJRpgkSN66/5SsT875jP0ZAqAgd8oGCgPwkNaN+BXwvAZJdJ0WTnfsDFovHAfPgonOhW6r0ABhN86i60tCmnVUcxhKQaJDKHK0EuP7VNB9C0rwHtLCqwyrTkz/yO/mIaT1GqdQN6phsAAAAAMAAAAAAAAABTYWx0ZWRfX2x0xTdMrar3KVl6j6q8doX8imUrqv2IcYWXacLIVtd4vIsajVEzrA4=");

    auto extractor =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive);
    infinit::cryptography::Code code(extractor);
    infinit::cryptography::Clear clear = k.decrypt(code);
    elle::String const output(clear.buffer().string());

    BOOST_CHECK_EQUAL(_input1, output);
  }

  // Decrypt complex type from [representation 3].
  {
    elle::String archive("AAAAAMgAAAAAAAAAAAAAAIAAAAAAAAAAurbHn1hwOB7ONw/RFe5YsPDOG9vgCsVOsJ6cPPuOYKxsRYL25wEULa5uNXxEbMnp+3d5ZbCT9b4LyoDJcSBZwa5nFGei1Z0YlkCRowJ9QB/7P52PUm5iNv208cB43CkNyU9Z2KURllFP80EgUH7f2hytcbzYh1pv4qDvuB0uJb0AAAAAMAAAAAAAAABTYWx0ZWRfXynUcAnpJAS6xu6c23Ecj2rm2O4MOq7WYyVaExK3cJ+cw3ayUZsUfQY=");

    auto extractor =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive);
    infinit::cryptography::Code code(extractor);
    Sample output = k.decrypt<Sample>(code);

    BOOST_CHECK_EQUAL(_input2, output);
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

    elle::String archive;
    elle::serialize::to_string(archive) << k1;

    auto extractor = elle::serialize::from_string(archive);
    infinit::cryptography::rsa::PrivateKey k2(extractor);

    BOOST_CHECK_EQUAL(k1, k2);
  }

  // For every hard-coded strings [representation 1] in every format,
  // deserialize the public keys, re-serialize them, make sure they can
  // be used and finally upgrade them: useful for detecting changes in formats.
  {
    std::vector<elle::String> const archives{
      // format 0
      "AAAAAAAAAACAAAAAwoyaKV59NC3T9YrWmkzkNrZPYaSuMbYc0VWFfKyJj243biEXp2Ka4pGz7T81U9Pb5vPofT+unLfcAZcmQfJ3SjocTSAJ1zdQKwNl37RWNjxSCyffABZhULTyucCKptoCz23Ntxhfrmrz8bA6g5FDrYrPuxXncc7BGa7ZVZCjOjMAAAMAAAABAAEAAIAAAAAzvd1KX10BQab7yP8mbojLnFUkbP3/LqYfbpVgAStcwD1xxs9ZrznuRD4MPSFbuB7mH7yXgxZXi/axXZfJNklPN8J9ztBoSCLdW07nYy1arISgtPZ3MNBkWmgBghMKXB6ulCBYTN07vtbSqguw7Qujdlj5jf8Db1Hr2pMaeuSqcQAAQAAAAOTKKTUE8r+KjCoi8sLS9FdqEIBwnCuzKYhqVah7TWRI+JmAxb+nSGp0y0shembi4N3NRFn+ZuXVxGQT8j07ah8AAEAAAADZr/CcWcTh07LKoQgdH4hdpW10xaj8sW0D33zxmbodYY7hAZctk2glLWDWcwaC0R7lSPbhnngAmszPabG6DZVtAABAAAAAaAkJjeNvIuw0amCz9SWKGEnRsSYDzddNdour4c4X1hdcfCjTQyAlTMtJOiPCDhDy4ckaYLXPQonfIQb06yUeTwAAQAAAAEOCEHgsTqZJ5JR6Q+3kLYH9H2irM7DJxSkCUdkK93DNJGYa/CsiH5YH8JQQr+A5wf8RFZ76afSmDQX1nSw5sjEAAEAAAABKe9dCMtKqXn7s8naFyPMgbT5Ss/g0nRA5CJ/s5ktCgl5zhi3Xd6w+SRIA+iWFL+Vto7p180YuReY0DCVBsHGu",
      // format 1
      "AAAAAAEAAACAAAAA296T6bIMwVQNuAmjVcllzjXp+sW2hBjFXcEKvyf6kNF12h+Kl8dpbA9c8fSl73pubtKX97q3hf80yjuQCRedd+BX96uGE8N8SI478kgXRs95jP7X3M+iD0ljYaijbZuEErblE3rkWF+Nknws0tQQHDH91wEqIzeZLm8DFpFIXxcAAAMAAAABAAEAAIAAAADNE+ZS68gJ6ySArDy1H4zdslvzk8aRn6k3sX8cnWWQ/k42jYTv3QfvysSeydqcbE3lAHjOkFHtogTTPIs0b9pm4s3T1YYRvNQ0oTPrb7GifxgnsYXijJDyHAF/Mul82VcsKK6eD7WYUM8mc2cHrc3MiOAU3lVjMK2i3mQqpAZYAQAAQAAAAPgD/A4AFgaVSCAnsK03+uGQ26Rg9ve2NK9g5Lwtu86g/yYQXvozLqYdMcqQRlpvQCGb8yue4y/3aAZEr6lLl5cAAEAAAADi8qCjrhkQiR5tLBdgHpNSearYv8jJIbMUjOCcjRS5Z5fWijttnP5pO+GBAi9zAVb+Z7qhq88hl/ont7JsFmSBAABAAAAAqphbDOpEPDz8wplqboud9v/kBNppphPfg27bhxDSKRwWxQDnw6PnB333XMUGj3gMndGeXzfIP5snI4Clb4a+ewAAQAAAAE49m4gSg22h+C1nH//xSpUKgQ4MwGonBhg1oVIXiWeKAVzjdy2JDGS3Uj0pxbX+ZkF6/WqzUsugG4wJ1qdQ6QEAAEAAAAB4DdvaKhxRfqFZuim3vgjbDjcnliPXdKa5sy4MUIYFWkqlBTBZL2ihl1eBpJx+/hjt47qai9vmnjcpMGJT9/PC"
      // format 2
      "AgAAAGACAAAAAAAAMIICXAIBAAKBgQDYvPRs+PsQRLs4+n9JXsRN2FWOetpxjmRikrgaNbnIcqoxpLrVq0sjDna3KNiHOmQI1bprBhoAGnDDspdd0NOqinkzJCfyjIKmiQtmQ/XOMcSy5weUjtvST9u06i6Pq/9df0lAicTcxmrXGX/gXpq5OY8SyWnM+0tEw2044d+YwwIDAQABAoGBALHzyrhiYTkFRbdNrQ7fuUu1EOIwKoTMBc4Naw1SCVz0EJs8Bo8C2mlJKd3OsqGtJo8+dJYE53ooO2fijXdgyzgaTGHMeAQJxNKv4lJdP7sfCPS7gSTn5Ob39Ap3sgCe/K0Jy5Y/URqEV95sIOszgQ5TfMSAUIoW1Pv0C5mGqaBpAkEA/Ei/qP+FdavFKhTp39LgldGhikEu6qxQWP2dUZE2w154U+UIV2hK70fR2UTgTCFkErri/+gKByLJO3S8ps9yLwJBANvuLX5Cv1HvDSf/pJ5SMCZPCGC/hnCZpRmDGgdNTykWXnOy4/WtWWdq8RkM3oxfA7piZ5t3uwy2KzCfsmKGwa0CQBakiN8oH9hR1F5UhjEagiGwMOlxtD5HvOZfzNDXUazzO6V79uWxbJhRP9paPi9XB95Oe7UwJEZDrv/xGEV4dO0CQClcSJf3K43YHCxKioXvxPhfYp73RexFKOR8YFfBNULv3wF5Qh/A4xBs7zyby/qwH75cpUl6oeXJGKRDVEPtPd0CQGF2zpA4h1RnEMdkl6RL433zP49vqjzZDACaonI1SxSyi7OXcFRAbM30+I2wjKwqOAf1haAd2hB0Se/C25GcKXsDAAUABAA="
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
