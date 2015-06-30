#include "cryptography.hh"
#include "Sample.hh"

#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/KeyPair.hh>
#include <cryptography/Exception.hh>

#include <elle/printf.hh>
#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

static elle::String const _input1("chiche, donne nous tout!");
static Sample const _input2(3240913, "Dave");

/*---------.
| Generate |
`---------*/

static
infinit::cryptography::rsa::PrivateKey
test_generate_rsa(elle::Natural32 const length = 1024)
{
  infinit::cryptography::rsa::KeyPair keypair =
    infinit::cryptography::rsa::keypair::generate(length);

  return (keypair.k());
}

static
void
test_generate()
{
  // RSA.
  test_generate_rsa();
}

/*----------.
| Construct |
`----------*/

static
void
test_construct()
{
  // RSA.
  infinit::cryptography::rsa::PrivateKey k1 = test_generate_rsa(2048);

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
test_operate_rsa()
{
  // Construct a private key from [representation 1]
  elle::String representation("AAAAAAAAAACAAAAAx9fdgyrca5Gg+aLMR4dyRSR/vlqeQ78NC01Me+XCrfIrOuooryGrNKf7piW3z2YFgZe3YlVLn8GHZzi95XjOi20NnPJhQzbK8wajTKRFqaTyAT78EpyUG13kMCPz+xNO5/Hgz/skK5wfMvP2fJgpxSn5coL7Bo7rNyMOKjXmzEEAAAMAAAABAAEAAIAAAABorhlZMOVe71yMBwvMWBWyRCkp89u9DuFtTyNGLxcn+JdOeAOxoQhGP8vgvxcma4Fs3aTwvE8tsrkTh7Ro9Dvz/bIPybLCF3pO7f2h6olNWttnKJdcCMkd0J8kkjRaST4sley0bwiepZOQH9GJ1dVUYV+ZicSSsCXc7LwliWIjNQAAQAAAAOh6Z+sGgDstX9mHyRfRdVqkm8e6gJ9D+aeuQXYjPuR7O4MJHvDgX2DwuQPE0bRguhAAh2P694nR9uX0m7D1SZMAAEAAAADcEChBmVwBAQ8e9t6fcD7J8y2rtOJx2Ktjth797d/WzErLYJ5zfHuUXHt6o/HuqCIG+pbrJESHHJlELwYspudbAABAAAAAoNDkB/GUTNtjfZ/vnJlZW92YoX5DMWkdT0A8qlNRU05n6YoqrCgZ8n4zDgddpTBwJVeoUKIwEFd9Oq+N78iOHwAAQAAAAGf1eg8DU+NljK8kEKdhb0gYCUtEeHUv1keUka+D4Gqot+y8CB6mf/rmda7KJFCKPQju+Ly8tWLer8j2LgSKFGEAAEAAAACNOU246B/0VNFBc+G1ekxdltjhWvgT9LE3yT4QH/3KMFK8zdvuX3WlUxWa3nlNZJjchXxHne/q2HB4h5vEcbGb");
  auto extractor =
    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(representation);
  infinit::cryptography::rsa::PrivateKey k(extractor);

  // The following operations are based on hard-coded base64 string which
  // represent the data on which to operate, in their serialized form.

  // Decrypt plain from [representation 2]
  {
    elle::String archive("AAAAAMgAAAAAAAAAAAAAAIAAAAAAAAAAA9ibq2o4RAMl1Bb5e2WyyuyeSolNUzIY5TqZdUo0KBKmE0ai6YQywDeFxnAE3HRfGrj761VdCrS/6dSA7tU9w587SirsgjWAlE1SWWaBsjP9Jl8Rfp2hIuJ8EzLPZ6noWCrBvEOkHE1Jvz8NWZwSfsXR9dOxt6lMKY4IasVC5NoAAAAAMAAAAAAAAABTYWx0ZWRfX5vQEsFfBQC7INk8+vj/6UXAiW+svkIJe8sg43ov+mBexbP/7BxGCLM=");

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
    elle::String archive("AAAAAMgAAAAAAAAAAAAAAIAAAAAAAAAAmbULAfuGhAbd9Thi+YEHtd3mfZcpyahJSYztbQQvGQa36fPGlxBA1bO121APzIcOIPGMsgfxUX00XW7q7HcOB0lAkkQhRKf6PWQv01KsGJClHZbBwaBwhnmO7N86jNYsHetwqUrvWdYg6LR97khSZ53cxbBamj29ER2WgsuvcwsAAAAAMAAAAAAAAABTYWx0ZWRfXxc+NsmHDzLCrXSBFML7bb77xDZidYiBnjIISqphIrC4RkBkSoo3H6w=");

    auto extractor =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive);
    infinit::cryptography::Code code(extractor);
    Sample output = k.decrypt<Sample>(code);

    BOOST_CHECK_EQUAL(_input2, output);
  }
}

static
void
test_operate()
{
  // RSA.
  test_operate_rsa();
}

/*--------.
| Compare |
`--------*/

static
void
test_compare_rsa()
{
  infinit::cryptography::rsa::PrivateKey k1 = test_generate_rsa(1024);
  infinit::cryptography::rsa::PrivateKey k2 = test_generate_rsa(1024);

  // With high probabilituy, this should not be the case. Otherwise,
  // the random generator is probably broken.
  BOOST_CHECK(k1 != k2);
  BOOST_CHECK(!(k1 == k2));
}

static
void
test_compare()
{
  // RSA.
  test_compare_rsa();
}

/*----------.
| Serialize |
`----------*/

static
void
test_serialize_rsa()
{
  // Serialize/deserialize.
  {
    infinit::cryptography::rsa::PrivateKey k1 = test_generate_rsa(2048);

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
      "AAAAAAEAAACAAAAA296T6bIMwVQNuAmjVcllzjXp+sW2hBjFXcEKvyf6kNF12h+Kl8dpbA9c8fSl73pubtKX97q3hf80yjuQCRedd+BX96uGE8N8SI478kgXRs95jP7X3M+iD0ljYaijbZuEErblE3rkWF+Nknws0tQQHDH91wEqIzeZLm8DFpFIXxcAAAMAAAABAAEAAIAAAADNE+ZS68gJ6ySArDy1H4zdslvzk8aRn6k3sX8cnWWQ/k42jYTv3QfvysSeydqcbE3lAHjOkFHtogTTPIs0b9pm4s3T1YYRvNQ0oTPrb7GifxgnsYXijJDyHAF/Mul82VcsKK6eD7WYUM8mc2cHrc3MiOAU3lVjMK2i3mQqpAZYAQAAQAAAAPgD/A4AFgaVSCAnsK03+uGQ26Rg9ve2NK9g5Lwtu86g/yYQXvozLqYdMcqQRlpvQCGb8yue4y/3aAZEr6lLl5cAAEAAAADi8qCjrhkQiR5tLBdgHpNSearYv8jJIbMUjOCcjRS5Z5fWijttnP5pO+GBAi9zAVb+Z7qhq88hl/ont7JsFmSBAABAAAAAqphbDOpEPDz8wplqboud9v/kBNppphPfg27bhxDSKRwWxQDnw6PnB333XMUGj3gMndGeXzfIP5snI4Clb4a+ewAAQAAAAE49m4gSg22h+C1nH//xSpUKgQ4MwGonBhg1oVIXiWeKAVzjdy2JDGS3Uj0pxbX+ZkF6/WqzUsugG4wJ1qdQ6QEAAEAAAAB4DdvaKhxRfqFZuim3vgjbDjcnliPXdKa5sy4MUIYFWkqlBTBZL2ihl1eBpJx+/hjt47qai9vmnjcpMGJT9/PC"};

    infinit::cryptography::test::formats<
      infinit::cryptography::rsa::PrivateKey>(archives);
  }
}

static
void
test_serialize()
{
  // RSA.
  test_serialize_rsa();
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("_legacy/PrivateyKey");

  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_compare));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}
