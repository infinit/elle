#include "test.hh"

#include <cryptography/KeyPair.hh>
#include <cryptography/PublicKey.hh>
#include <cryptography/PrivateKey.hh>
#include <cryptography/Exception.hh>

#include <elle/types.hh>
#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

/*----------.
| Represent |
`----------*/

void
test_represent_rsa()
{
  // 1)
  {
    infinit::cryptography::KeyPair pair =
      infinit::cryptography::KeyPair::generate(
        infinit::cryptography::Cryptosystem::rsa,
        1024);
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << pair;
    elle::printf("[representation 1] %s\n", archive);
  }
}

void
test_represent()
{
  // These generate base64-based representations which can be used in
  // other tests.

  // RSA.
  test_represent_rsa();
}

/*---------.
| Generate |
`---------*/

infinit::cryptography::KeyPair
test_generate_rsa(elle::Natural32 const length = 2048)
{
  infinit::cryptography::KeyPair pair =
    infinit::cryptography::KeyPair::generate(
      infinit::cryptography::Cryptosystem::rsa,
      length);

  return (pair);
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

infinit::cryptography::KeyPair
test_construct_rsa()
{
  infinit::cryptography::KeyPair pair =
    test_generate_rsa(1024);

  return (pair);
}

void
test_construct()
{
  // RSA.
  infinit::cryptography::KeyPair pair1 =
    test_construct_rsa();

  // KeyPair copy.
  infinit::cryptography::KeyPair pair2(pair1);

  BOOST_CHECK_EQUAL(pair1, pair2);

  // KeyPair move.
  infinit::cryptography::KeyPair pair3(std::move(pair1));

  BOOST_CHECK_EQUAL(pair2, pair3);

  // Attributes copy.
  infinit::cryptography::KeyPair pair4(pair2.K(), pair2.k());

  BOOST_CHECK_EQUAL(pair2, pair3);
  BOOST_CHECK_EQUAL(pair2, pair4);
  BOOST_CHECK_EQUAL(pair3, pair4);

  // Attributes move.
  infinit::cryptography::PublicKey K(pair3.K());
  infinit::cryptography::PrivateKey k(pair3.k());

  infinit::cryptography::KeyPair pair5(std::move(K), std::move(k));

  BOOST_CHECK_EQUAL(pair2, pair3);
  BOOST_CHECK_EQUAL(pair2, pair4);
  BOOST_CHECK_EQUAL(pair2, pair5);
  BOOST_CHECK_EQUAL(pair3, pair4);
  BOOST_CHECK_EQUAL(pair3, pair5);
  BOOST_CHECK_EQUAL(pair4, pair5);
}

/*--------.
| Operate |
`--------*/

void
test_operate_rsa()
{
  infinit::cryptography::KeyPair pair =
    test_generate_rsa(1024);

  // Public/private encryption/decryption with plain.
  {
    elle::String input =
      infinit::cryptography::random::generate<elle::String>(9128);
    infinit::cryptography::Code code =
      pair.K().encrypt(
        infinit::cryptography::Plain{
          elle::WeakBuffer{reinterpret_cast<void*>(const_cast<char*>(input.c_str())),
                                                   input.length()}});
    infinit::cryptography::Clear clear = pair.k().decrypt(code);
    elle::String const output(reinterpret_cast<char const*>(clear.buffer().contents()),
                              clear.buffer().size());

    BOOST_CHECK_EQUAL(input, output);
  }

  // Public/private encryption/decryption with complex type.
  {
    Class const input(
      42, infinit::cryptography::random::generate<elle::String>(14920));
    infinit::cryptography::Code code = pair.K().encrypt(input);
    Class const output = pair.k().decrypt<Class>(code);

    BOOST_CHECK_EQUAL(input, output);
  }

  // Private/public encryption/decryption with plain.
  {
    elle::String const input =
      infinit::cryptography::random::generate<elle::String>(5123);
    infinit::cryptography::Code code =
      pair.k().encrypt(
        infinit::cryptography::Plain{
          elle::WeakBuffer{reinterpret_cast<void*>(const_cast<char*>(input.c_str())),
                                                   input.length()}});
    infinit::cryptography::Clear clear = pair.K().decrypt(code);
    elle::String const output(reinterpret_cast<char const*>(clear.buffer().contents()),
                              clear.buffer().size());

    BOOST_CHECK_EQUAL(input, output);
  }

  // Private/public encryption/decryption with complex type.
  {
    Class const input(
      84, infinit::cryptography::random::generate<elle::String>(28130));
    infinit::cryptography::Code code = pair.k().encrypt(input);
    Class const output = pair.K().decrypt<Class>(code);

    BOOST_CHECK_EQUAL(input, output);
  }

  // Sign a plain text.
  {
    elle::String input =
      infinit::cryptography::random::generate<elle::String>(1493);
    infinit::cryptography::Signature signature =
      pair.k().sign(
        infinit::cryptography::Plain{
          elle::WeakBuffer{reinterpret_cast<void*>(const_cast<char*>(input.c_str())),
                                                   input.length()}});
    auto result = pair.K().verify(signature,
                                  infinit::cryptography::Plain{
                                    elle::WeakBuffer{reinterpret_cast<void*>(const_cast<char*>(input.c_str())),
                                                                             input.length()}});

    BOOST_CHECK_EQUAL(result, true);
  }

  // Sign a complex type.
  {
    Class const input(
      84, infinit::cryptography::random::generate<elle::String>(10329));
    infinit::cryptography::Signature signature = pair.k().sign(input);
    auto result = pair.K().verify(signature, input);

    BOOST_CHECK_EQUAL(result, true);
  }
}

void
test_operate()
{
  // RSA.
  test_operate_rsa();
}

/*----------.
| Serialize |
`----------*/

void
test_serialize_rsa()
{
  // Serialize/deserialize.
  {
    infinit::cryptography::KeyPair pair1 = test_generate_rsa(512);

    elle::String archive;
    elle::serialize::to_string(archive) << pair1;

    auto extractor = elle::serialize::from_string(archive);
    infinit::cryptography::KeyPair pair2(extractor);

    BOOST_CHECK(pair1 == pair2);
  }

  // Deserialize from the hard-coded string [representation 1]: useful
  // for detecting changes in formats.
  {
    elle::String archive1("AAAAAAAAAAAAAIAAAADTJND1LjjrbBkBAU0jaMPk4rrD5952vb4FKQYLiD2DrBnK/jnQQU97VV9UvSllSBfvL8wfDuy6o/eV4z6bIzEjR/a4whwu02xsHAyiW5vrNTLWXgKO8VaLjk1YAAgbQUleh4L3RyiDJiZDyWORZEzqIORT/u686fc8/sz7pxBj5wAAAwAAAAEAAQAAAAAAAAAAgAAAANMk0PUuOOtsGQEBTSNow+TiusPn3na9vgUpBguIPYOsGcr+OdBBT3tVX1S9KWVIF+8vzB8O7Lqj95XjPpsjMSNH9rjCHC7TbGwcDKJbm+s1MtZeAo7xVouOTVgACBtBSV6HgvdHKIMmJkPJY5FkTOog5FP+7rzp9zz+zPunEGPnAAADAAAAAQABAACAAAAAUL5Vgk1Iyw6tiSrcOtxHfXIPBffXpQTuc766ZxKUo5Pe0TfjF/bB1GJTcG5eNDMbpAZdQstMUBuABLvAJ0eP9EII9g55Ve8pmM7Hs+1oRNy7xZSiNuo2QG4othiE5poNNuoXEIY7Cq15Gr3fzXJahX83BcpvvtMphOmubtWsXCEAAEAAAADyhUC02fZZ3AaQps+V/HDwjBmPbY0rltI5QGc30FmUItaZRVrL95FuFvD97tkt9ZvDrizt3NPmh627Qfma8VtpAABAAAAA3uEdRQQ73G0oBydIKwMBrxojvwmU0YZU1gzV91vITmF1sb/nPUY46PMRRJNuiuDBlpecViqtmHwVAJ+Iwd5qzwAAQAAAAFvAIfzJwTO51BoBoeNif+/hcQIcNh5Zjurt/J7XCizLCVkvLeygmrqFn2LSbAPBOc7MJF9Q/jHm/skrap091pEAAEAAAACSs3fxkpzYNN0RAFEnRn5YRAtdVjAnTZUPozk+7KHGva2bRuYs1cjlxAAMtEIhlVi3B6n2qXbEFIA6fcelXAHPAABAAAAAfWLqKlb/jubacUHv8r9HD2lDFSH2z+UA8SpxilOm3F0sEMArPImHeaivWeKWThedAEUv6z0FiFryrx1Lzeg5vg==");

    auto extractor =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive1);
    infinit::cryptography::KeyPair pair(extractor);

    elle::String archive2;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive2) << pair;

    BOOST_CHECK_EQUAL(archive1, archive2);
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
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("KeyPair");

  // To uncomment if one wants to update the representations.
  //suite->add(BOOST_TEST_CASE(test_represent));

  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_serialize));
  // XXX[rotate/derive]

  boost::unit_test::framework::master_test_suite().add(suite);

  return (true);
}

int
main(int argc,
     char** argv)
{
  return boost::unit_test::unit_test_main(test, argc, argv);
}
