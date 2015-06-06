#include "../cryptography.hh"
#include "../Sample.hh"

#include <cryptography/dsa/PublicKey.hh>
#include <cryptography/dsa/PrivateKey.hh>
#include <cryptography/dsa/KeyPair.hh>
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
  infinit::cryptography::dsa::KeyPair keypair =
    infinit::cryptography::dsa::keypair::generate(2048);

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
infinit::cryptography::dsa::PublicKey
_test_generate(elle::Natural32 const length = 1024)
{
  infinit::cryptography::dsa::KeyPair keypair =
    infinit::cryptography::dsa::keypair::generate(length);

  infinit::cryptography::dsa::PublicKey K(keypair.K());

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
  infinit::cryptography::dsa::PublicKey K1 = _test_generate(2048);

  // PublicKey copy.
  infinit::cryptography::dsa::PublicKey K2(K1);

  BOOST_CHECK_EQUAL(K1, K2);

  // PublicKey move.
  infinit::cryptography::dsa::PublicKey K3(std::move(K1));

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
  elle::String representation("AAAAACkDAAAAAAAAMIIDJQKCAQAmbAd1mlvkvOoykKU2OS1LxuM7mm7psoYTLZzBKX9tOVJiLsa2ho14pj8BrZmmSpMa5q2Mh6U2C+lrrvAg7q5R8U+guLFzneD4ZCYrzqHH/jpKbWVvh+oo48mgB9exDB0WhlrFy71aFtJQ1lgvx6RcwdSRT/mmxoIaXNYyMNvpxOtqpgNCvcywtMITzSpdzRWIG7eNFSJA54+GEejQSWuOxkkb69pXnz1jD1TRC1p1y3IGYRalEIpPl8IgajgO54CmRTouB/+DnU+fLjfyJBQpY7CTx2Kwg/ficNVCKOHJw1NkJhwFxFMa/5+rSlwwt2Cp5UH2b9ybQenWOd93ETlbAoIBAQD+l8CvnBfo8vQqMAY8azL259cV8SRcDrhInbPQxpSImvuEUzs853eEkaQmGewNUB1wgDl+gfqa97Bp01Hiwj6f+X5Xo0XqaV82m4CTBxZFpu82YqTgggxJfZxnD6Sqd2VFxfJsO3t1qOYxNCZ9GaBhHOs22swIg2gjOc0Q4I9yjgC3UBFglcvSBcsWffZEmbuwH/Dy1/wHVegb6gewvsF6ps6dGYRYiYYXrxeTrSSc0beZrR/M0AkQU+KUWXnc5l4eYUGuAFjPo1+W5aPPQrXWOLfwIeLbRNKjIj/i3KBVaEFtcmffGk/iCpjft5AtmfWMrtK1nusc/0JGToYU3fDhAhUAuJ+7UMJEg1JKxVk1glkjPMBfCNMCggEBAOcd5Or/DJBGQENVQDEYk2B+iKf+HptEj1xZxYUgI3Yitb2rs2yu1VaSo3njfJsx/wlpFLYrdiRb9fVZjf/M0Bic9vky3CuKLG15KfjFn8OdD7yOqR2FXXjA0S7Ka8STlIjl19o0WmBnkfQNbf9Aip78Wlo1D12P+WF2VOJ+/ZKopJv28sLc4tTTu4tceLpprQtjbgzDIvsAEhM/ZC1mudiokZLOqdqzA3uBssz9i1jh6EXBftdIn9lEaO2WE9d6hGzyBPUDMBNAQr/Ts7izIonbcb9oy20BFJx/VZFletmGO1KrBf0rlKWjkuUoLtQb6jaItAtvV1DoP7NEgEiLr2AEAA==");

  auto extractor =
    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(representation);
  infinit::cryptography::dsa::PublicKey K(extractor);

  // The following operations are based on hard-coded base64 string which
  // represent the data on which to operate, in their serialized form.

  // Verify plain from [representation 2].
  {
    elle::String archive("AAAAAC8AAAAAAAAAMC0CFELZjhQI0BH3FMtCkMCL1+XW7NhpAhUAqVVQ+Jl3tp9QZ0ENuPG5JA2a53o=");

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
    elle::String archive("AAAAAC4AAAAAAAAAMCwCFCQ7xQntbDtNaALrHXio7jg6ATmAAhRvc6k2v/EcVwXEi+MnhBz6+9re/A==");

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
  infinit::cryptography::dsa::PublicKey K1 = _test_generate(1024);
  infinit::cryptography::dsa::PublicKey K2 = _test_generate(1024);

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
    infinit::cryptography::dsa::PublicKey K1 = _test_generate(2048);

    elle::String archive;
    elle::serialize::to_string(archive) << K1;

    auto extractor = elle::serialize::from_string(archive);
    infinit::cryptography::dsa::PublicKey K2(extractor);

    BOOST_CHECK_EQUAL(K1, K2);
  }

  // For every hard-coded strings [representation 1] in every format,
  // deserialize the public keys, re-serialize them, make sure they can
  // be used and finally upgrade them: useful for detecting changes in formats.
  {
    std::vector<elle::String> const archives{
      // format 0
      "AAAAACkDAAAAAAAAMIIDJQKCAQAmbAd1mlvkvOoykKU2OS1LxuM7mm7psoYTLZzBKX9tOVJiLsa2ho14pj8BrZmmSpMa5q2Mh6U2C+lrrvAg7q5R8U+guLFzneD4ZCYrzqHH/jpKbWVvh+oo48mgB9exDB0WhlrFy71aFtJQ1lgvx6RcwdSRT/mmxoIaXNYyMNvpxOtqpgNCvcywtMITzSpdzRWIG7eNFSJA54+GEejQSWuOxkkb69pXnz1jD1TRC1p1y3IGYRalEIpPl8IgajgO54CmRTouB/+DnU+fLjfyJBQpY7CTx2Kwg/ficNVCKOHJw1NkJhwFxFMa/5+rSlwwt2Cp5UH2b9ybQenWOd93ETlbAoIBAQD+l8CvnBfo8vQqMAY8azL259cV8SRcDrhInbPQxpSImvuEUzs853eEkaQmGewNUB1wgDl+gfqa97Bp01Hiwj6f+X5Xo0XqaV82m4CTBxZFpu82YqTgggxJfZxnD6Sqd2VFxfJsO3t1qOYxNCZ9GaBhHOs22swIg2gjOc0Q4I9yjgC3UBFglcvSBcsWffZEmbuwH/Dy1/wHVegb6gewvsF6ps6dGYRYiYYXrxeTrSSc0beZrR/M0AkQU+KUWXnc5l4eYUGuAFjPo1+W5aPPQrXWOLfwIeLbRNKjIj/i3KBVaEFtcmffGk/iCpjft5AtmfWMrtK1nusc/0JGToYU3fDhAhUAuJ+7UMJEg1JKxVk1glkjPMBfCNMCggEBAOcd5Or/DJBGQENVQDEYk2B+iKf+HptEj1xZxYUgI3Yitb2rs2yu1VaSo3njfJsx/wlpFLYrdiRb9fVZjf/M0Bic9vky3CuKLG15KfjFn8OdD7yOqR2FXXjA0S7Ka8STlIjl19o0WmBnkfQNbf9Aip78Wlo1D12P+WF2VOJ+/ZKopJv28sLc4tTTu4tceLpprQtjbgzDIvsAEhM/ZC1mudiokZLOqdqzA3uBssz9i1jh6EXBftdIn9lEaO2WE9d6hGzyBPUDMBNAQr/Ts7izIonbcb9oy20BFJx/VZFletmGO1KrBf0rlKWjkuUoLtQb6jaItAtvV1DoP7NEgEiLr2AEAA=="
    };

    infinit::cryptography::test::formats<
      infinit::cryptography::dsa::PublicKey>(archives);
  }
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("dsa/PublicyKey");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_compare));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}
