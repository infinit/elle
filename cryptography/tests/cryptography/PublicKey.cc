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

elle::String const _input1("my balls are ok!");
Sample const _input2(23293083121, "chodaboy");

void
test_represent_rsa()
{
  infinit::cryptography::KeyPair pair =
    infinit::cryptography::KeyPair::generate(
      infinit::cryptography::Cryptosystem::rsa,
      2048);

  // 1)
  {
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << pair.K();
    elle::printf("[representation 1] %s\n", archive);
  }

  // 2)
  {
    infinit::cryptography::Code code =
      pair.k().encrypt(
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
    infinit::cryptography::Code code = pair.k().encrypt(_input2);
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << code;
    elle::printf("[representation 3] %s\n", archive);
  }

  // 4)
  {
    infinit::cryptography::Signature signature =
      pair.k().sign(
        infinit::cryptography::Plain(
          elle::WeakBuffer{reinterpret_cast<void*>(const_cast<char*>(_input1.c_str())),
                                                   _input1.length()}));
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << signature;
    elle::printf("[representation 4] %s\n", archive);
  }

  // 5)
  {
    infinit::cryptography::Signature signature = pair.k().sign(_input2);
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << signature;
    elle::printf("[representation 5] %s\n", archive);
  }
}

void
test_represent()
{
  // These generate base64-based representations which can be used in
  // other tests.

  test_represent_rsa();
}

/*---------.
| Generate |
`---------*/

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

  // Decrypt plain from [representation 2]
  {
    elle::String archive("AAAAAEgBAAAAAAAAAAAAAAABAAAAAAAAPp7Cjg0v+ri4JgUPpEJF2rn+b2BcZdY8tFixtPqEeW+GWjF8bDV/6URur/6SlqfNsTOd/QBfWughvuySXG9e2xTqMPNnr5vxrR93AUFLOzVqpzh4l2L06rcek6O8wrgLGUo5sdQVmOK4bcCaKMBHr07dX65v+p0VJbXBL1v6BbB2vwr96oON83do/nhhWUl6OCNKc8uNUNFsoezWbGuB/M6isEnWGL0bwcysIDR7rMBdQFddyMl+mnRGZPLwUrtQAN5nD87eCE6G6VRks7EYQIVD16ixFZsmOr7jJj+27vfS+ecudYqx6rkmt/pkkM9vlT1adazZ2H5xWBzvC8H00QAAAAAwAAAAAAAAAFNhbHRlZF9fPJ74dkTTB2ejro62W8At35SZxkz6HY942Y0N+b2G73pO+/SUSI1aaA==");

    auto extractor =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive);
    infinit::cryptography::Code code(extractor);

    infinit::cryptography::Clear clear = K.decrypt(code);
    elle::String const output(reinterpret_cast<char const*>(clear.buffer().contents()),
                              clear.buffer().size());

    BOOST_CHECK_EQUAL(_input1, output);
  }

  // Decrypt complex type from [representation 3].
  {
    elle::String archive("AAAAAEgBAAAAAAAAAAAAAAABAAAAAAAAvv3h5tCR1eqnGNTapIOO0kDAsjSHy9KLuDwyVaK120XrTtE3vAiLUUqedtqrRcisEk+dbiYL4IWGqj3LunM5/yWwf/R2DK4MKIR/YOvxLSkjLqggqzgFqGEvA5C5LcVeGuOE+qlEOmR2WryKzM0GHfsV7GWG5+IPpWI4J2PdZGitIJQ2t0uBKaOEYPaMiFXy0ohABWgFSnZaAxreqlOW74K4fmGniQQ7tWOHeE5rgyaG8NnsCsorLupYBj3D0yR7onohQnQWKBWjJxUnyhMOa5EfPSw4dT5QzlgBDu4evuOiZcnQBk6QuYpnzXDfClhDDttrVzoDBdzsbmUvgFMInAAAAAAwAAAAAAAAAFNhbHRlZF9fV0ATftOFs0xIEVCJh8APAASPmedeCOyiuCYumgjW7zeuquwPJYOC5Q==");

    auto extractor =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive);
    infinit::cryptography::Code code(extractor);

    Sample output = K.decrypt<Sample>(code);

    BOOST_CHECK_EQUAL(_input2, output);
  }

  // Verify plain from [representation 4].
  {
    elle::String archive("AAAAAAABAAAAAAAACtl3p9rCYGLPwr6YbEkmo17d5QtogRLppEIzHbtA+P1WAZYFuQeSF4/BE5eJZugYu4PEbVgdX5Mv8l11nddGfdgmwuEL1ACZ6yc7gxps2Skb2U+ZzbrS3ZBYeGYe5haUfjrVCCyP2wE/oe86WiAKhBVIEekb7eAxtO+AlQm6JjBzmTHcDK9ZxaLwmrPxhUyLMTfyTRWf53di7njr65IppRdJMfgbQB+hDWdODSDj/M446e+C8NQWIKxj5EjqQC62dEoyH9CAht98cOHL6251EZRZEG+YFiSTdmo77QHwpAZ+Mq7erlYW78g1uPgDQ1kJ8olMyGYF3tWIazQ1reEonA==");

    auto extractor =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive);
    infinit::cryptography::Signature signature(extractor);

    auto result =
      K.verify(signature,
               infinit::cryptography::Plain(
                 elle::WeakBuffer{reinterpret_cast<void*>(const_cast<char*>(_input1.c_str())),
                                                          _input1.length()}));

    BOOST_CHECK_EQUAL(result, true);
  }

  // Verify plain from [representation 5].
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
    infinit::cryptography::PublicKey K1 = test_generate_rsa(2048);

    elle::String archive;
    elle::serialize::to_string(archive) << K1;

    auto extractor = elle::serialize::from_string(archive);
    infinit::cryptography::PublicKey K2(extractor);

    BOOST_CHECK_EQUAL(K1, K2);
  }

  // Deserialize from the hard-coded string [representation 1]: useful
  // for detecting changes in formats.
  {
    elle::String archive1("AAAAAAAAAACAAAAApcZBa1TEkWy17WfZVvNGHDLufkKpu3HRBZ/r4pPtdCKufOBKYw22eWxKEClEo06IfnuCtzfn/ZfSGDMelzJKy5XhJ+qX1e7IYTioWcUq928awLyK/4mAShE3lv5BQPDx983+1LEigaYmRcD1Dii1sSMyezvxCVyufYWD2NOQjMMAAAMAAAABAAE=");

    auto extractor =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive1);
    infinit::cryptography::PublicKey K1(extractor);

    elle::String archive2;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive2) << K1;

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
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("PublicyKey");

  // To uncomment if one wants to update the representations.
  //suite->add(BOOST_TEST_CASE(test_represent));

  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_compare));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);

  return (true);
}

int
main(int argc,
     char** argv)
{
  return (boost::unit_test::unit_test_main(test, argc, argv));
}
