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
  elle::String representation("AAAAAGECAAAAAAAAMIICXQIBAAKBgQDUHnc3TP+empgq+TToKaDriDBKV7mxflQkEuLI3HL7pBnFE5Z34WQWX/zwSDpkrKQ+MQrRuX4NSDoCVumzr0caUAtYepqEVDif0l3sLIgL4HuY2E0sALBOMEG82pTn0Ly6X4GllmGyzMbS8M22rkU18MkYKMrLIzy1q9+RmwFH6QIDAQABAoGARjFatAMX560yw62500XLSYXErSVAfrrWLyXMxDknF5yjJnMn7L26OClFc2IDaMM0ipwB/4rJaq3G/HU0WtCaz0ClnOSWpCvj07Fb2uXSF46LassxtJGQ1vqoNvUUsJK4vYNBrkf8t/uxPgRn6E8/KipqmW4e9Nk61rWywsdhiTECQQD6OHy3M53CpgG+YVDGm7+E25X7v1BEFbxqsi0PTJFua3I/7GXGSNhDQ50uyYVIauRBRYb3Ds7W1EFaArgfPfk1AkEA2QSwmrfPagQ6IPoQ6fFJe2EB8gUw3b7WbndhFLio1RA/wRBKf+wXkodDJerIGgQvIKYw5eYdj5EAvMrXo+reZQJBAM9/RlT6FcpTtcB1nPGGOLTpD6BbjylWyQ/Fv74fJb7mlTDTTba3s7db+53P+ZoLIkvKZ1i7S+U00RjehmdKEj0CQGz7JPDdY4R60PIkgf/pUpxpz79JCgwhPK/GtXLhdK9idJGwa6kn14KW+Ip/B7PA7CW6L44T7D5fc/5a4sWHOd0CQQDw1r92khpoIJB9YqJXbNxSvmXsNDnBqtwDn4G2IDf1qCh8b5Bba7T/HsOeN6/DVvruf1X3immzBKRew4kfPZ1MAwAFAAQA");
  auto extractor =
    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(representation);
  infinit::cryptography::rsa::PrivateKey k(extractor);

  // The following operations are based on hard-coded base64 string which
  // represent the data on which to operate, in their serialized form.

  // Decrypt plain from [representation 2]
  {
    elle::String archive("AAAAAMgAAAAAAAAAAAAAAIAAAAAAAAAADRHZIGx8CrvHD39VLvs6ptByu3buV3scJ3twAIRQMYTEA+wQq5Ptuwkb+lRb4PitaTDfFmVmyAPTOCyntCOh8iUAVUK8eOhIngVpnerdLgB+Md7+XCrDBug116WvKY5+xd5yC3Nz0cndeazdFgeKAtrW/HyQuRTvyEI301DcG7oAAAAAMAAAAAAAAABTYWx0ZWRfX0agB2y3yp5cxIRucpLuIfAx1QmQvSfrfy6OdkYBU7HNN+N0zMgFzyU=");

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
    elle::String archive("AAAAAMgAAAAAAAAAAAAAAIAAAAAAAAAALhXoFivuPUo7Bze4i1UsmI4eEzqZX14ndoqmshSFw1N13N6Op9/reGqBQt9kLUMYjidlXtFmYW+Eg1pGMFJohiEeZ5R8TUijmfIJXa9kOtevtg9s79qIlsTprrYtDfTddESsnk+mMrsVH83oSzEyIg+7h1izsnVrH9W2o2HuAewAAAAAMAAAAAAAAABTYWx0ZWRfX3UJCoKnlMVH4K4dYYvELN2L0is6aVo0d25htREgRPVTLvHEUG6Ijp8=");

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
      "AAAAAGECAAAAAAAAMIICXQIBAAKBgQDUHnc3TP+empgq+TToKaDriDBKV7mxflQkEuLI3HL7pBnFE5Z34WQWX/zwSDpkrKQ+MQrRuX4NSDoCVumzr0caUAtYepqEVDif0l3sLIgL4HuY2E0sALBOMEG82pTn0Ly6X4GllmGyzMbS8M22rkU18MkYKMrLIzy1q9+RmwFH6QIDAQABAoGARjFatAMX560yw62500XLSYXErSVAfrrWLyXMxDknF5yjJnMn7L26OClFc2IDaMM0ipwB/4rJaq3G/HU0WtCaz0ClnOSWpCvj07Fb2uXSF46LassxtJGQ1vqoNvUUsJK4vYNBrkf8t/uxPgRn6E8/KipqmW4e9Nk61rWywsdhiTECQQD6OHy3M53CpgG+YVDGm7+E25X7v1BEFbxqsi0PTJFua3I/7GXGSNhDQ50uyYVIauRBRYb3Ds7W1EFaArgfPfk1AkEA2QSwmrfPagQ6IPoQ6fFJe2EB8gUw3b7WbndhFLio1RA/wRBKf+wXkodDJerIGgQvIKYw5eYdj5EAvMrXo+reZQJBAM9/RlT6FcpTtcB1nPGGOLTpD6BbjylWyQ/Fv74fJb7mlTDTTba3s7db+53P+ZoLIkvKZ1i7S+U00RjehmdKEj0CQGz7JPDdY4R60PIkgf/pUpxpz79JCgwhPK/GtXLhdK9idJGwa6kn14KW+Ip/B7PA7CW6L44T7D5fc/5a4sWHOd0CQQDw1r92khpoIJB9YqJXbNxSvmXsNDnBqtwDn4G2IDf1qCh8b5Bba7T/HsOeN6/DVvruf1X3immzBKRew4kfPZ1MAwAFAAQA"
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
