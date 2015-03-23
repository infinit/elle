#include "../cryptography.hh"
#include "../Sample.hh"

#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/keypair.hh>
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
        infinit::cryptography::Plain{
          elle::ConstWeakBuffer{_input1}});
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
  elle::String representation("AAAAAGECAAAAAAAAMIICXQIBAAKBgQDXrTDsHnXO1twuCTX0IRv+W+D1EuboN2WWRkkLBje3fYvOmgF6eMpoLxpLLLPzhHPNGS9tiGjo2PAgSGe1SoZX/GCuz/I9g70sxMb80bbqddp04tZwVoWma2qYQhSsie2Oox5P8AK1Smdj5YiFhlMP6UJP+Cc18rdo3bNJ+uPUcQIDAQABAoGAaSeO78CWLcfCSEvDW4j9DhIb50FVBalkxVT1bgOVDMiQbqXVzLGx9gPpLPfZsQRGNZxu7FeS1txWj1beAZ0ZTE0Py7SVkvno+jSPaOxzz6diT4H53Ze5xoTjvHNUTKuvRqpvgWNcmFd0T71EKnML1lypFbAZAAjBy8mqbNpy2AECQQD8aT9wQhjoIv8X5edicrePg/KqsDhIEj57K9DjpUyDj66K7LM2jdd8bHMASCLx93vIt8ppKpmkDxtKUr9zgXHxAkEA2r47n2tfnl3fd9grt2ocEpps36oSc9Y2kpf4l7igbnzxM7JjAjxMNuGO5BUhvyFYiBbw8FUNKbdu0iGvRZYKgQJBAMGoVn3gmI9yWlmRkMed0GwSBlsjPOs+5CVIZspiBqRh1r7MAGv4w724zzsGHZmcEaLcMDYj8yFGXd/WQQFz7WECQC9pfkjeoBoWZMJ4M3mM4UrS9/g+x8Md0jRSBkr0dKmeITXAW593bFmU6EDdp7PiyWqt9klRVgoNvVKEhJYDoIECQQDOBSkLpRCrsG34ZT92orEoWrAYfCefmTpsXR4x1YXaJ/jt3Sux8dsPRz70pUwUK4gT/RZn7dW+Ai4HMTxN44Z+");
  auto extractor =
    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(representation);
  infinit::cryptography::rsa::PrivateKey k(extractor);

  // The following operations are based on hard-coded base64 string which
  // represent the data on which to operate, in their serialized form.

  // Decrypt plain from [representation 2]
  {
    elle::String archive("AAAAAMgAAAAAAAAAAAAAAIAAAAAAAAAA1t5a0aEbqkvARHQKeYszhTEE4D/22EddU5QrdX3SmrgFZKdZ+9xUu6ALUnYzJXNDBuDbChNLZJoXjDMvIPWa4ZFuw9/HJfrDQ46/Hzhprm7lzYkQ0CpdfBR+U1dFsa5nVsClL8ZhzHf4peYukkxEAjrFT1sQQ3VuDDmwkKClmYkAAAAAMAAAAAAAAABTYWx0ZWRfX9ysSlcWDb0GROzez5PFH1E5lUZTsuTt3vqvABDhnXKbx3c5qqOxjpo=");

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
    elle::String archive("AAAAAMgAAAAAAAAAAAAAAIAAAAAAAAAAI2Y0AqdresrexCaFrqQ+ei5QHGWYLCF0tTb9byVB2nBZmYs3h9+kRMaDkhMe/EP1lymSK+5lcy4N4S4OG3rq1VM0sH1IHkEc+T+Lcze4zqeZIaN4HkmLtFhQ58uCwcxkjdBZ3hNMRmDVNeQnXJNvd1fKp2ZsjJpel2GyAKzb33wAAAAAMAAAAAAAAABTYWx0ZWRfX4J1pKfbEj220eHVLH5s8/NtEiLlMoL9pGN5G7LuKni8LHlGGdvOtps=");

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
      "AAAAAGECAAAAAAAAMIICXQIBAAKBgQDXrTDsHnXO1twuCTX0IRv+W+D1EuboN2WWRkkLBje3fYvOmgF6eMpoLxpLLLPzhHPNGS9tiGjo2PAgSGe1SoZX/GCuz/I9g70sxMb80bbqddp04tZwVoWma2qYQhSsie2Oox5P8AK1Smdj5YiFhlMP6UJP+Cc18rdo3bNJ+uPUcQIDAQABAoGAaSeO78CWLcfCSEvDW4j9DhIb50FVBalkxVT1bgOVDMiQbqXVzLGx9gPpLPfZsQRGNZxu7FeS1txWj1beAZ0ZTE0Py7SVkvno+jSPaOxzz6diT4H53Ze5xoTjvHNUTKuvRqpvgWNcmFd0T71EKnML1lypFbAZAAjBy8mqbNpy2AECQQD8aT9wQhjoIv8X5edicrePg/KqsDhIEj57K9DjpUyDj66K7LM2jdd8bHMASCLx93vIt8ppKpmkDxtKUr9zgXHxAkEA2r47n2tfnl3fd9grt2ocEpps36oSc9Y2kpf4l7igbnzxM7JjAjxMNuGO5BUhvyFYiBbw8FUNKbdu0iGvRZYKgQJBAMGoVn3gmI9yWlmRkMed0GwSBlsjPOs+5CVIZspiBqRh1r7MAGv4w724zzsGHZmcEaLcMDYj8yFGXd/WQQFz7WECQC9pfkjeoBoWZMJ4M3mM4UrS9/g+x8Md0jRSBkr0dKmeITXAW593bFmU6EDdp7PiyWqt9klRVgoNvVKEhJYDoIECQQDOBSkLpRCrsG34ZT92orEoWrAYfCefmTpsXR4x1YXaJ/jt3Sux8dsPRz70pUwUK4gT/RZn7dW+Ai4HMTxN44Z+"
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
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("PrivateyKey");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_compare));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}
