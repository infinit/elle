#include "test.hh"

#include <cryptography/PrivateKey.hh>
#include <cryptography/PublicKey.hh>
#include <cryptography/Exception.hh>
#include <cryptography/rsa/keypair.hh>

#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

/*---------.
| Generate |
`---------*/

infinit::cryptography::PrivateKey
test_generate_rsa(elle::Natural32 const length = 1024)
{
  // By implementation.
  std::pair<infinit::cryptography::rsa::PublicKey,
            infinit::cryptography::rsa::PrivateKey> pair =
    infinit::cryptography::rsa::keypair::generate(length);

  std::unique_ptr<infinit::cryptography::privatekey::Interface> interface(
    new infinit::cryptography::rsa::PrivateKey{std::move(pair.second)});

  infinit::cryptography::PrivateKey k(std::move(interface));

  return (k);
}

void
test_generate()
{
  // RSA.
  test_generate_rsa();
}

/*----------.
| Construct |
`----------*/

void
test_construct()
{
  // RSA.
  infinit::cryptography::PrivateKey k1 = test_generate_rsa(2048);

  // PrivateKey copy.
  infinit::cryptography::PrivateKey k2(k1);

  BOOST_CHECK_EQUAL(k1, k2);

  // PrivateKey move.
  infinit::cryptography::PrivateKey k3(std::move(k1));

  BOOST_CHECK_EQUAL(k2, k3);
}

/*----------.
| Represent |
`----------*/

elle::String const _input1("chiche, donne nous tout!");
Class const _input2(3240913, "Dave");

void
test_represent_rsa()
{
  infinit::cryptography::KeyPair pair =
    infinit::cryptography::KeyPair::generate(
      infinit::cryptography::Cryptosystem::rsa,
      1024);

  // 1)
  {
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << pair.k();
    elle::printf("[representation 1] %s\n", archive);
  }

  // 2)
  {
    infinit::cryptography::Code code =
      pair.K().encrypt(
        infinit::cryptography::Plain(
          elle::WeakBuffer{reinterpret_cast<void*>(const_cast<char*>(_input1.c_str())),
                                                   _input1.length()}));
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << code;
    elle::printf("[representation 2] %s\n", archive);
  }

  // 3)
  {
    infinit::cryptography::Code code = pair.K().encrypt(_input2);
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << code;
    elle::printf("[representation 3] %s\n", archive);
  }
}

void
test_represent()
{
  // These generate base64-based representations which can be used in
  // other tests.
  test_represent_rsa();
}

/*--------.
| Operate |
`--------*/

void
test_operate_rsa()
{
  // Construct a private key from [representation 1]
  elle::String representation("AAAAAAAAAACAAAAAx9fdgyrca5Gg+aLMR4dyRSR/vlqeQ78NC01Me+XCrfIrOuooryGrNKf7piW3z2YFgZe3YlVLn8GHZzi95XjOi20NnPJhQzbK8wajTKRFqaTyAT78EpyUG13kMCPz+xNO5/Hgz/skK5wfMvP2fJgpxSn5coL7Bo7rNyMOKjXmzEEAAAMAAAABAAEAAIAAAABorhlZMOVe71yMBwvMWBWyRCkp89u9DuFtTyNGLxcn+JdOeAOxoQhGP8vgvxcma4Fs3aTwvE8tsrkTh7Ro9Dvz/bIPybLCF3pO7f2h6olNWttnKJdcCMkd0J8kkjRaST4sley0bwiepZOQH9GJ1dVUYV+ZicSSsCXc7LwliWIjNQAAQAAAAOh6Z+sGgDstX9mHyRfRdVqkm8e6gJ9D+aeuQXYjPuR7O4MJHvDgX2DwuQPE0bRguhAAh2P694nR9uX0m7D1SZMAAEAAAADcEChBmVwBAQ8e9t6fcD7J8y2rtOJx2Ktjth797d/WzErLYJ5zfHuUXHt6o/HuqCIG+pbrJESHHJlELwYspudbAABAAAAAoNDkB/GUTNtjfZ/vnJlZW92YoX5DMWkdT0A8qlNRU05n6YoqrCgZ8n4zDgddpTBwJVeoUKIwEFd9Oq+N78iOHwAAQAAAAGf1eg8DU+NljK8kEKdhb0gYCUtEeHUv1keUka+D4Gqot+y8CB6mf/rmda7KJFCKPQju+Ly8tWLer8j2LgSKFGEAAEAAAACNOU246B/0VNFBc+G1ekxdltjhWvgT9LE3yT4QH/3KMFK8zdvuX3WlUxWa3nlNZJjchXxHne/q2HB4h5vEcbGb");
  auto extractor =
    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(representation);
  infinit::cryptography::PrivateKey k(extractor);

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
    elle::String const output(reinterpret_cast<char const*>(clear.buffer().contents()),
                              clear.buffer().size());

    BOOST_CHECK_EQUAL(_input1, output);
  }

  // Decrypt complex type from [representation 3].
  {
    elle::String archive("AAAAAMgAAAAAAAAAAAAAAIAAAAAAAAAAmbULAfuGhAbd9Thi+YEHtd3mfZcpyahJSYztbQQvGQa36fPGlxBA1bO121APzIcOIPGMsgfxUX00XW7q7HcOB0lAkkQhRKf6PWQv01KsGJClHZbBwaBwhnmO7N86jNYsHetwqUrvWdYg6LR97khSZ53cxbBamj29ER2WgsuvcwsAAAAAMAAAAAAAAABTYWx0ZWRfXxc+NsmHDzLCrXSBFML7bb77xDZidYiBnjIISqphIrC4RkBkSoo3H6w=");

    auto extractor =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive);
    infinit::cryptography::Code code(extractor);
    Class output = k.decrypt<Class>(code);

    BOOST_CHECK_EQUAL(_input2, output);
  }
}

void
test_operate()
{
  // RSA.
  test_operate_rsa();
}

/*--------.
| Compare |
`--------*/

void
test_compare_rsa()
{
  infinit::cryptography::PrivateKey k1 = test_generate_rsa(1024);
  infinit::cryptography::PrivateKey k2 = test_generate_rsa(1024);

  // With high probabilituy, this should not be the case. Otherwise,
  // the random generator is probably broken.
  BOOST_CHECK(k1 != k2);
  BOOST_CHECK(!(k1 == k2));

  if (k1 < k2)
    {
      BOOST_CHECK(k1 <= k2);
      BOOST_CHECK(!(k1 > k2));
      BOOST_CHECK(!(k1 >= k2));
    }
  else
    {
      BOOST_CHECK(k1 >= k2);
      BOOST_CHECK(!(k1 < k2));
      BOOST_CHECK(!(k1 <= k2));
    }
}

void
test_compare()
{
  // RSA.
  test_compare_rsa();
}

/*----------.
| Serialize |
`----------*/

void
test_serialize_rsa()
{
  // Serialize/deserialize.
  {
    infinit::cryptography::PrivateKey k1 = test_generate_rsa(2048);

    elle::String archive;
    elle::serialize::to_string(archive) << k1;

    auto extractor = elle::serialize::from_string(archive);
    infinit::cryptography::PrivateKey k2(extractor);

    BOOST_CHECK_EQUAL(k1, k2);
  }

  // Deserialize from the hard-coded string [representation 1]: useful
  // for detecting changes in formats.
  {
    elle::String archive1("AAAAAAAAAACAAAAAwoyaKV59NC3T9YrWmkzkNrZPYaSuMbYc0VWFfKyJj243biEXp2Ka4pGz7T81U9Pb5vPofT+unLfcAZcmQfJ3SjocTSAJ1zdQKwNl37RWNjxSCyffABZhULTyucCKptoCz23Ntxhfrmrz8bA6g5FDrYrPuxXncc7BGa7ZVZCjOjMAAAMAAAABAAEAAIAAAAAzvd1KX10BQab7yP8mbojLnFUkbP3/LqYfbpVgAStcwD1xxs9ZrznuRD4MPSFbuB7mH7yXgxZXi/axXZfJNklPN8J9ztBoSCLdW07nYy1arISgtPZ3MNBkWmgBghMKXB6ulCBYTN07vtbSqguw7Qujdlj5jf8Db1Hr2pMaeuSqcQAAQAAAAOTKKTUE8r+KjCoi8sLS9FdqEIBwnCuzKYhqVah7TWRI+JmAxb+nSGp0y0shembi4N3NRFn+ZuXVxGQT8j07ah8AAEAAAADZr/CcWcTh07LKoQgdH4hdpW10xaj8sW0D33zxmbodYY7hAZctk2glLWDWcwaC0R7lSPbhnngAmszPabG6DZVtAABAAAAAaAkJjeNvIuw0amCz9SWKGEnRsSYDzddNdour4c4X1hdcfCjTQyAlTMtJOiPCDhDy4ckaYLXPQonfIQb06yUeTwAAQAAAAEOCEHgsTqZJ5JR6Q+3kLYH9H2irM7DJxSkCUdkK93DNJGYa/CsiH5YH8JQQr+A5wf8RFZ76afSmDQX1nSw5sjEAAEAAAABKe9dCMtKqXn7s8naFyPMgbT5Ss/g0nRA5CJ/s5ktCgl5zhi3Xd6w+SRIA+iWFL+Vto7p180YuReY0DCVBsHGu");

    auto extractor =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive1);
    infinit::cryptography::PrivateKey k1(extractor);

    elle::String archive2;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive2) << k1;

    BOOST_CHECK(archive1 == archive2);
  }
}

void
test_serialize()
{
  // RSA.
  test_serialize_rsa();
}

/*-----.
| Main |
`-----*/

bool
test()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("PrivateyKey");

  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_compare));
  suite->add(BOOST_TEST_CASE(test_serialize));

  // To uncomment if one wants to update the representations.
  //suite->add(BOOST_TEST_CASE(test_represent));

  boost::unit_test::framework::master_test_suite().add(suite);

  return (true);
}

int
main(int argc,
     char** argv)
{
  return boost::unit_test::unit_test_main(test, argc, argv);
}
