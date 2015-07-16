#include "cryptography.hh"
#include "Sample.hh"

#include <cryptography/rsa/KeyPair.hh>
#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/random.hh>

#include <elle/printf.hh>
#include <elle/types.hh>
#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

/*---------.
| Generate |
`---------*/

static
infinit::cryptography::rsa::KeyPair
test_generate_rsa(elle::Natural32 const length = 2048)
{
  infinit::cryptography::rsa::KeyPair pair =
    infinit::cryptography::rsa::keypair::generate(length);

  return (pair);
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
infinit::cryptography::rsa::KeyPair
test_construct_rsa()
{
  infinit::cryptography::rsa::KeyPair pair = test_generate_rsa(1024);

  return (pair);
}

static
void
test_construct()
{
  // RSA.
  infinit::cryptography::rsa::KeyPair pair1 = test_construct_rsa();

  // KeyPair copy.
  infinit::cryptography::rsa::KeyPair pair2(pair1);

  BOOST_CHECK_EQUAL(pair1, pair2);

  // KeyPair move.
  infinit::cryptography::rsa::KeyPair pair3(std::move(pair1));

  BOOST_CHECK_EQUAL(pair2, pair3);

  // Attributes copy.
  infinit::cryptography::rsa::KeyPair pair4(pair2.K(), pair2.k());

  BOOST_CHECK_EQUAL(pair2, pair3);
  BOOST_CHECK_EQUAL(pair2, pair4);
  BOOST_CHECK_EQUAL(pair3, pair4);

  // Attributes move.
  infinit::cryptography::rsa::PublicKey K(pair3.K());
  infinit::cryptography::rsa::PrivateKey k(pair3.k());

  infinit::cryptography::rsa::KeyPair pair5(std::move(K), std::move(k));

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

static
void
_test_operate_rsa(infinit::cryptography::rsa::KeyPair const& pair)
{
  // Public/private encryption/decryption with plain.
  {
    elle::String input =
      infinit::cryptography::random::generate<elle::String>(9128);
    infinit::cryptography::Code code =
      pair.K().encrypt(
        infinit::cryptography::Plain{elle::ConstWeakBuffer{input}});
    infinit::cryptography::Clear clear = pair.k().decrypt(code);
    elle::String const output(clear.buffer().string());

    BOOST_CHECK_EQUAL(input, output);
  }

  // Public/private encryption/decryption with complex type.
  {
    Sample const input(
      42, infinit::cryptography::random::generate<elle::String>(14920));
    infinit::cryptography::Code code = pair.K().encrypt(input);
    Sample const output = pair.k().decrypt<Sample>(code);

    BOOST_CHECK_EQUAL(input, output);
  }

  // Sign a plain text.
  {
    elle::String input =
      infinit::cryptography::random::generate<elle::String>(1493);
    infinit::cryptography::Signature signature =
      pair.k().sign(
        infinit::cryptography::Plain{elle::ConstWeakBuffer{input}});
    auto result =
      pair.K().verify(signature,
                      infinit::cryptography::Plain{
                        elle::ConstWeakBuffer{input}});

    BOOST_CHECK_EQUAL(result, true);
  }

  // Sign a complex type.
  {
    Sample const input(
      84, infinit::cryptography::random::generate<elle::String>(10329));
    infinit::cryptography::Signature signature = pair.k().sign(input);
    auto result = pair.K().verify(signature, input);

    BOOST_CHECK_EQUAL(result, true);
  }
}

static
void
test_operate_rsa()
{
  infinit::cryptography::rsa::KeyPair pair = test_generate_rsa(1024);

  _test_operate_rsa(pair);
}

static
void
test_operate()
{
  // RSA.
  test_operate_rsa();
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
    infinit::cryptography::rsa::KeyPair pair1 = test_generate_rsa(512);

    elle::String archive;
    elle::serialize::to_string(archive) << pair1;

    auto extractor = elle::serialize::from_string(archive);
    infinit::cryptography::rsa::KeyPair pair2(extractor);

    BOOST_CHECK_EQUAL(pair1, pair2);

    _test_operate_rsa(pair2);
  }

  // For every hard-coded strings [representation 1] in every format,
  // deserialize the key pairs, re-serialize them, make sure they can
  // be used and finally upgrade them: useful for detecting changes in formats.
  {
    std::vector<elle::String> const archives{
      // format 0
      "AAAAAAAAAAAAAIAAAADTJND1LjjrbBkBAU0jaMPk4rrD5952vb4FKQYLiD2DrBnK/jnQQU97VV9UvSllSBfvL8wfDuy6o/eV4z6bIzEjR/a4whwu02xsHAyiW5vrNTLWXgKO8VaLjk1YAAgbQUleh4L3RyiDJiZDyWORZEzqIORT/u686fc8/sz7pxBj5wAAAwAAAAEAAQAAAAAAAAAAgAAAANMk0PUuOOtsGQEBTSNow+TiusPn3na9vgUpBguIPYOsGcr+OdBBT3tVX1S9KWVIF+8vzB8O7Lqj95XjPpsjMSNH9rjCHC7TbGwcDKJbm+s1MtZeAo7xVouOTVgACBtBSV6HgvdHKIMmJkPJY5FkTOog5FP+7rzp9zz+zPunEGPnAAADAAAAAQABAACAAAAAUL5Vgk1Iyw6tiSrcOtxHfXIPBffXpQTuc766ZxKUo5Pe0TfjF/bB1GJTcG5eNDMbpAZdQstMUBuABLvAJ0eP9EII9g55Ve8pmM7Hs+1oRNy7xZSiNuo2QG4othiE5poNNuoXEIY7Cq15Gr3fzXJahX83BcpvvtMphOmubtWsXCEAAEAAAADyhUC02fZZ3AaQps+V/HDwjBmPbY0rltI5QGc30FmUItaZRVrL95FuFvD97tkt9ZvDrizt3NPmh627Qfma8VtpAABAAAAA3uEdRQQ73G0oBydIKwMBrxojvwmU0YZU1gzV91vITmF1sb/nPUY46PMRRJNuiuDBlpecViqtmHwVAJ+Iwd5qzwAAQAAAAFvAIfzJwTO51BoBoeNif+/hcQIcNh5Zjurt/J7XCizLCVkvLeygmrqFn2LSbAPBOc7MJF9Q/jHm/skrap091pEAAEAAAACSs3fxkpzYNN0RAFEnRn5YRAtdVjAnTZUPozk+7KHGva2bRuYs1cjlxAAMtEIhlVi3B6n2qXbEFIA6fcelXAHPAABAAAAAfWLqKlb/jubacUHv8r9HD2lDFSH2z+UA8SpxilOm3F0sEMArPImHeaivWeKWThedAEUv6z0FiFryrx1Lzeg5vg==",
      // format 1
      "AQAAAAEAAAAAAQAAAIAAAACiHJRGs5Zo30viHP59jHo66w4Hi1jkfmzubJN5bdQueTjdIk8C3LhbyTuUi0HW7cj3fzJgc93GP6GzJ6bFucUb1b0/hy2VykdAJ0hevPbITHzmzFHyxzZhwV8UWKT/MicL2oj1ra0ULBwrYeVddRDzApSvlQCRAj4U5qb96PEVPQAAAwAAAAEAAQAAAQAAAAABAAAAgAAAAKIclEazlmjfS+Ic/n2MejrrDgeLWOR+bO5sk3lt1C55ON0iTwLcuFvJO5SLQdbtyPd/MmBz3cY/obMnpsW5xRvVvT+HLZXKR0AnSF689shMfObMUfLHNmHBXxRYpP8yJwvaiPWtrRQsHCth5V11EPMClK+VAJECPhTmpv3o8RU9AAADAAAAAQABAACAAAAARTaohAs3A62ay36PlXD92tvy0HvJRz1HKwDGckjXF8R/58I3M8SjHeUr//747Ff3vtva2V1+6Z7PXhtm0/K92bd+7nUg5LnhrJkUrBg72oJEbm3D3JnhIpH426i3qB6ikwgWJc6iXRxWhmCRrySDqI6Y7UilLXYJNBuEB1qobu0AAEAAAADR0SJ++4ulnyHewXCFKBwqOfdI7T/SEK/zvCmpZ0psJzHPYNwUPesimf2fM1HT9pUYoJtX/HxwYMlOnlMIFV9HAABAAAAAxctS1k1qOo5QRFM+8Olj2JD4g56Sm9+CCPbTgTM2sldD1AN6Lcs9I+8xRd1123d56rVYUglJAqE2ScyIKReRWwAAQAAAAINRu59yXNLKKG6872nH0ujy4ry4EOuvGhEYwZKPXVsXpOtAmj3pCxPbAYv0mNBrUqHZU6DcHB5R1WzpVtqGVd0AAEAAAAClS7sJjx60mtQnlW4nRNSFWvnkfPPGwe3Y8fVlptgSCG2N0+4aMRwCWNXTrjTdWSaQ6EZMIKhcQz+sQvqTZLe7AABAAAAAQMkWndgWi+meBmhEWTIJEERlEQmTGRzsJU8sATuVnLrmbhpmNoklCJAGjCg+fDdte9Zh7RwMAnd9XT7Ce7R/1g=="};

    infinit::cryptography::test::formats<infinit::cryptography::rsa::KeyPair>(
      archives, std::bind(_test_operate_rsa, std::placeholders::_1));
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
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("_legacy/KeyPair");

  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}
