#include "cryptography.hh"
#include "Sample.hh"

#include <cryptography/PublicKey.hh>
#include <cryptography/PrivateKey.hh>
#include <cryptography/Exception.hh>
#include <cryptography/rsa/keypair.hh>

#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

/*----------.
| Represent |
`----------*/

static elle::String const _input1("my balls are ok!");
static Sample const _input2(23293083121, "chodaboy");

static
void
test_represent_rsa()
{
  infinit::cryptography::KeyPair keypair =
    infinit::cryptography::KeyPair::generate(
      infinit::cryptography::Cryptosystem::rsa,
      2048);

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
        infinit::cryptography::Plain{
          elle::ConstWeakBuffer{_input1}});
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

  test_represent_rsa();
}

/*---------.
| Generate |
`---------*/
#ifdef interface
# undef interface
#endif

static
infinit::cryptography::PublicKey
test_generate_rsa(elle::Natural32 const length = 1024)
{
  // By implementation.
  std::pair<infinit::cryptography::rsa::PublicKey,
            infinit::cryptography::rsa::PrivateKey> pair =
    infinit::cryptography::rsa::keypair::generate(length);

  std::unique_ptr<infinit::cryptography::publickey::Interface> interface(
    new infinit::cryptography::rsa::PublicKey{std::move(pair.first)});

  infinit::cryptography::PublicKey K(std::move(interface));

  return (K);
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
  infinit::cryptography::PublicKey K1 = test_generate_rsa(2048);

  // PublicKey copy.
  infinit::cryptography::PublicKey K2(K1);

  BOOST_CHECK_EQUAL(K1, K2);

  // PublicKey move.
  infinit::cryptography::PublicKey K3(std::move(K1));

  BOOST_CHECK_EQUAL(K2, K3);
}

/*--------.
| Operate |
`--------*/

static
void
test_operate_rsa()
{
  // Construct a public key from [representation 1]
  elle::String representation("AAAAAAAAAAAAAQAAwyqSEVWQVKNMVyKy+COV5huZNfdkU79dKbyV3Pt7rvVJ2FNcdx2AJOZc7iWRAcB9i+VdHBgJs9uJ/5wUoBvjiXBV45E+6wm0Zz6QRqdtph1vRP8UtXMfY7nZxh6h/NgcV5uGj/zwrg4PWZq1NR3DzA3r2n5buc4TLoX8sFUlSBF5wIUAWL/GLKD+ZR1tsoszXVgG+KR7RzTT0CqEzQ23KLtHoVQi3cXFE4hTFBsAxbhbO9WC1hLh13CcpmWPKBo8lCtbvcElmodQnuwouprZydICVDKoQjsRuj+AyOqjQgkhSN7eTHCpffaf0cjz+qMReboRSw9zhIp8uBqtCIAv+wAAAwAAAAEAAQ==");

  auto extractor =
    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(representation);
  infinit::cryptography::PublicKey K(extractor);

  // The following operations are based on hard-coded base64 string which
  // represent the data on which to operate, in their serialized form.

  // Verify plain from [representation 2].
  {
    elle::String archive("AAAAAAABAAAAAAAACtl3p9rCYGLPwr6YbEkmo17d5QtogRLppEIzHbtA+P1WAZYFuQeSF4/BE5eJZugYu4PEbVgdX5Mv8l11nddGfdgmwuEL1ACZ6yc7gxps2Skb2U+ZzbrS3ZBYeGYe5haUfjrVCCyP2wE/oe86WiAKhBVIEekb7eAxtO+AlQm6JjBzmTHcDK9ZxaLwmrPxhUyLMTfyTRWf53di7njr65IppRdJMfgbQB+hDWdODSDj/M446e+C8NQWIKxj5EjqQC62dEoyH9CAht98cOHL6251EZRZEG+YFiSTdmo77QHwpAZ+Mq7erlYW78g1uPgDQ1kJ8olMyGYF3tWIazQ1reEonA==");

    auto extractor =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive);
    infinit::cryptography::Signature signature(extractor);

    auto result =
      K.verify(signature,
               infinit::cryptography::Plain{
                 elle::ConstWeakBuffer{_input1}});

    BOOST_CHECK_EQUAL(result, true);
  }

  // Verify plain from [representation 3].
  {
    elle::String archive("AAAAAAABAAAAAAAABIZUvjdGrl1z9AHo4RYsrnvchBBnLnA+B3B4oLdfcDcp5Th0pe85rjyrwoTsFn/NgqsdMmitgDmOZ+cwc5SVMAoq4sN5jshz9bypaSAW+ZmHzoibvhL7eJjPmUJEKUN+mg4rpY6mMVu9btzaROcBocL8J2JjCxLSm3r3xd75k2idKLlbIndtjWt2atKErxTgBSEsYYyjUHS5OaO1ElZgftz8ooDZQZr7AKUYbY+ppydl3aaGiHpekHN1qgya5YC/MwDRVsbptTqeg8h0lWezWgP480mTSk2UgjyQTCcxa8l7dvsrQWpdythkrZLk+KaARci7S82LGIcqhxmMNDBOng==");

    auto extractor =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive);
    infinit::cryptography::Signature signature(extractor);

    auto result = K.verify(signature, _input2);

    BOOST_CHECK_EQUAL(result, true);
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
  infinit::cryptography::PublicKey K1 = test_generate_rsa(1024);
  infinit::cryptography::PublicKey K2 = test_generate_rsa(1024);

  // With high probabilituy, this should not be the case. Otherwise,
  // the random generator is probably broken.
  BOOST_CHECK(K1 != K2);
  BOOST_CHECK(!(K1 == K2));

  if (K1 < K2)
  {
    BOOST_CHECK(K1 <= K2);
    BOOST_CHECK(!(K1 > K2));
    BOOST_CHECK(!(K1 >= K2));
  }
  else
  {
    BOOST_CHECK(K1 >= K2);
    BOOST_CHECK(!(K1 < K2));
    BOOST_CHECK(!(K1 <= K2));
  }
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
    infinit::cryptography::PublicKey K1 = test_generate_rsa(2048);

    elle::String archive;
    elle::serialize::to_string(archive) << K1;

    auto extractor = elle::serialize::from_string(archive);
    infinit::cryptography::PublicKey K2(extractor);

    BOOST_CHECK_EQUAL(K1, K2);
  }

  // For every hard-coded strings [representation 1] in every format,
  // deserialize the public keys, re-serialize them, make sure they can
  // be used and finally upgrade them: useful for detecting changes in formats.
  {
    std::vector<elle::String> const archives{
      // format 0
      "AAAAAAAAAACAAAAApcZBa1TEkWy17WfZVvNGHDLufkKpu3HRBZ/r4pPtdCKufOBKYw22eWxKEClEo06IfnuCtzfn/ZfSGDMelzJKy5XhJ+qX1e7IYTioWcUq928awLyK/4mAShE3lv5BQPDx983+1LEigaYmRcD1Dii1sSMyezvxCVyufYWD2NOQjMMAAAMAAAABAAE=",
      // format 1
      "AAAAAAEAAAAAAQAAzJcO4BIlHIdoQUYGiMvjSG+U4434tyZOhRnfRJUxd9tK/zxezIwPQOBShjU5zin4rhXholbVKJxOLrMN29AoF+wJi3c8+GCFQFBH5sJAQwf66M6kIOdtY2rtIIUd8H4SxWNe9j7MpYQxaYWpMlJE578oZ2/X4aYOO/5Ki4jA4X+afRjaN3yJnikG7OkkwndRLzQzR2BTmG6zT5hGrOZQ13G1SmeMxI5REK6rjrAsrbNPi9yojrjGK5UyityKy2yjiJSf7br7f34sG1xIhKcUW1MWUxi0gaFR4X/6TbTeIxXuuRIgyY3OLWZx3O8Vlx7jwcWcPG/kY5RPbdfU29x9lwAAAwAAAAEAAQ=="};

    infinit::cryptography::test::formats<infinit::cryptography::PublicKey>(
      archives);
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
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("PublicyKey");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_compare));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}

