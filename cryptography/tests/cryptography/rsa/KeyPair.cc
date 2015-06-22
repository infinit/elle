#include "../cryptography.hh"
#include "../Sample.hh"

#include <cryptography/rsa/KeyPair.hh>
#include <cryptography/rsa/PublicKey.hh>
#include <cryptography/rsa/PrivateKey.hh>
#include <cryptography/rsa/Padding.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/Exception.hh>
#include <cryptography/random.hh>

#include <elle/printf.hh>
#include <elle/types.hh>
#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

/*----------.
| Represent |
`----------*/

static
void
_test_represent(elle::Natural32 const length,
                infinit::cryptography::rsa::Padding const encryption_padding,
                infinit::cryptography::rsa::Padding const signature_padding,
                infinit::cryptography::Oneway const digest_algorithm,
                infinit::cryptography::Cipher const envelope_cipher,
                infinit::cryptography::Mode const envelope_mode)
{
  // 1)
  {
    infinit::cryptography::rsa::KeyPair keypair =
      infinit::cryptography::rsa::keypair::generate(length,
                                                    encryption_padding,
                                                    signature_padding,
                                                    digest_algorithm,
                                                    envelope_cipher,
                                                    envelope_mode);

    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << keypair;
    elle::printf("[representation 1] %s\n", archive);
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

  _test_represent(1024,
                  infinit::cryptography::rsa::Padding::oaep,
                  infinit::cryptography::rsa::Padding::pss,
                  infinit::cryptography::Oneway::sha1,
                  infinit::cryptography::Cipher::idea,
                  infinit::cryptography::Mode::cbc);
}

/*---------.
| Generate |
`---------*/

static
infinit::cryptography::rsa::KeyPair
_test_generate(elle::Natural32 const length,
               infinit::cryptography::rsa::Padding const encryption_padding,
               infinit::cryptography::rsa::Padding const signature_padding,
               infinit::cryptography::Oneway const digest_algorithm,
               infinit::cryptography::Cipher const envelope_cipher,
               infinit::cryptography::Mode const envelope_mode)
{
  infinit::cryptography::rsa::KeyPair keypair =
    infinit::cryptography::rsa::keypair::generate(length,
                                                  encryption_padding,
                                                  signature_padding,
                                                  digest_algorithm,
                                                  envelope_cipher,
                                                  envelope_mode);

  return (keypair);
}

static
void
test_generate()
{
  _test_generate(4096,
                 infinit::cryptography::rsa::Padding::pkcs1,
                 infinit::cryptography::rsa::Padding::pkcs1,
                 infinit::cryptography::Oneway::sha256,
                 infinit::cryptography::Cipher::blowfish,
                 infinit::cryptography::Mode::ecb);
}

/*----------.
| Construct |
`----------*/

static
void
test_construct()
{
  infinit::cryptography::rsa::KeyPair keypair1 =
    _test_generate(2048,
                   infinit::cryptography::rsa::Padding::oaep,
                   infinit::cryptography::rsa::Padding::pkcs1,
                   infinit::cryptography::Oneway::sha,
                   infinit::cryptography::Cipher::aes128,
                   infinit::cryptography::Mode::ofb);

  // KeyPair copy.
  infinit::cryptography::rsa::KeyPair keypair2(keypair1);

  BOOST_CHECK_EQUAL(keypair1, keypair2);

  // KeyPair move.
  infinit::cryptography::rsa::KeyPair keypair3(std::move(keypair1));

  BOOST_CHECK_EQUAL(keypair2, keypair3);

  // Attributes copy.
  infinit::cryptography::rsa::KeyPair keypair4(keypair2.K(), keypair2.k());

  BOOST_CHECK_EQUAL(keypair2, keypair3);
  BOOST_CHECK_EQUAL(keypair2, keypair4);
  BOOST_CHECK_EQUAL(keypair3, keypair4);

  // Attributes move.
  infinit::cryptography::rsa::PublicKey K(keypair3.K());
  infinit::cryptography::rsa::PrivateKey k(keypair3.k());

  infinit::cryptography::rsa::KeyPair keypair5(std::move(K), std::move(k));

  BOOST_CHECK_EQUAL(keypair2, keypair3);
  BOOST_CHECK_EQUAL(keypair2, keypair4);
  BOOST_CHECK_EQUAL(keypair2, keypair5);
  BOOST_CHECK_EQUAL(keypair3, keypair4);
  BOOST_CHECK_EQUAL(keypair3, keypair5);
  BOOST_CHECK_EQUAL(keypair4, keypair5);
}

/*--------.
| Operate |
`--------*/

static
void
_test_operate(infinit::cryptography::rsa::KeyPair const& keypair)
{
  // Public/private encryption/decryption with plain.
  {
    elle::String input =
      infinit::cryptography::random::generate<elle::String>(9128);
    infinit::cryptography::Code code =
      keypair.K().encrypt(
        infinit::cryptography::Plain{input});
    infinit::cryptography::Clear clear = keypair.k().decrypt(code);
    elle::String const output(clear.buffer().string());

    BOOST_CHECK_EQUAL(input, output);
  }

  // Public/private encryption/decryption with complex type.
  {
    Sample const input(
      42, infinit::cryptography::random::generate<elle::String>(14920));
    infinit::cryptography::Code code = keypair.K().encrypt(input);
    Sample const output = keypair.k().decrypt<Sample>(code);

    BOOST_CHECK_EQUAL(input, output);
  }

  // Sign a plain text.
  {
    elle::String input =
      infinit::cryptography::random::generate<elle::String>(1493);
    infinit::cryptography::Signature signature =
      keypair.k().sign(
        infinit::cryptography::Plain{input});
    auto result =
      keypair.K().verify(signature,
                         infinit::cryptography::Plain{input});

    BOOST_CHECK_EQUAL(result, true);
  }

  // Sign a complex type.
  {
    Sample const input(
      84, infinit::cryptography::random::generate<elle::String>(10329));
    infinit::cryptography::Signature signature = keypair.k().sign(input);
    auto result = keypair.K().verify(signature, input);

    BOOST_CHECK_EQUAL(result, true);
  }
}

static
void
test_operate()
{
  infinit::cryptography::rsa::KeyPair keypair =
    _test_generate(512,
                   infinit::cryptography::rsa::Padding::pkcs1,
                   infinit::cryptography::rsa::Padding::pss,
                   infinit::cryptography::Oneway::md5,
                   infinit::cryptography::Cipher::desx,
                   infinit::cryptography::Mode::cbc);

  _test_operate(keypair);
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
    infinit::cryptography::rsa::KeyPair keypair1 =
      _test_generate(1024,
                     infinit::cryptography::rsa::Padding::oaep,
                     infinit::cryptography::rsa::Padding::pkcs1,
                     infinit::cryptography::Oneway::sha512,
                     infinit::cryptography::Cipher::des2,
                     infinit::cryptography::Mode::cfb);

    elle::String archive;
    elle::serialize::to_string(archive) << keypair1;

    auto extractor = elle::serialize::from_string(archive);

    infinit::cryptography::rsa::KeyPair keypair2(extractor);

    BOOST_CHECK_EQUAL(keypair1, keypair2);

    _test_operate(keypair2);
  }

  // For every hard-coded strings [representation 1] in every format,
  // deserialize the key pairs, re-serialize them, make sure they can
  // be used and finally upgrade them: useful for detecting changes in formats.
  {
    std::vector<elle::String> const archives{
      // format 0
      "AAAAAAAAAAAAAIAAAADTJND1LjjrbBkBAU0jaMPk4rrD5952vb4FKQYLiD2DrBnK/jnQQU97VV9UvSllSBfvL8wfDuy6o/eV4z6bIzEjR/a4whwu02xsHAyiW5vrNTLWXgKO8VaLjk1YAAgbQUleh4L3RyiDJiZDyWORZEzqIORT/u686fc8/sz7pxBj5wAAAwAAAAEAAQAAAAAAAAAAgAAAANMk0PUuOOtsGQEBTSNow+TiusPn3na9vgUpBguIPYOsGcr+OdBBT3tVX1S9KWVIF+8vzB8O7Lqj95XjPpsjMSNH9rjCHC7TbGwcDKJbm+s1MtZeAo7xVouOTVgACBtBSV6HgvdHKIMmJkPJY5FkTOog5FP+7rzp9zz+zPunEGPnAAADAAAAAQABAACAAAAAUL5Vgk1Iyw6tiSrcOtxHfXIPBffXpQTuc766ZxKUo5Pe0TfjF/bB1GJTcG5eNDMbpAZdQstMUBuABLvAJ0eP9EII9g55Ve8pmM7Hs+1oRNy7xZSiNuo2QG4othiE5poNNuoXEIY7Cq15Gr3fzXJahX83BcpvvtMphOmubtWsXCEAAEAAAADyhUC02fZZ3AaQps+V/HDwjBmPbY0rltI5QGc30FmUItaZRVrL95FuFvD97tkt9ZvDrizt3NPmh627Qfma8VtpAABAAAAA3uEdRQQ73G0oBydIKwMBrxojvwmU0YZU1gzV91vITmF1sb/nPUY46PMRRJNuiuDBlpecViqtmHwVAJ+Iwd5qzwAAQAAAAFvAIfzJwTO51BoBoeNif+/hcQIcNh5Zjurt/J7XCizLCVkvLeygmrqFn2LSbAPBOc7MJF9Q/jHm/skrap091pEAAEAAAACSs3fxkpzYNN0RAFEnRn5YRAtdVjAnTZUPozk+7KHGva2bRuYs1cjlxAAMtEIhlVi3B6n2qXbEFIA6fcelXAHPAABAAAAAfWLqKlb/jubacUHv8r9HD2lDFSH2z+UA8SpxilOm3F0sEMArPImHeaivWeKWThedAEUv6z0FiFryrx1Lzeg5vg==",
      // format 1
      "AQAAAAEAAAAAAQAAAIAAAACiHJRGs5Zo30viHP59jHo66w4Hi1jkfmzubJN5bdQueTjdIk8C3LhbyTuUi0HW7cj3fzJgc93GP6GzJ6bFucUb1b0/hy2VykdAJ0hevPbITHzmzFHyxzZhwV8UWKT/MicL2oj1ra0ULBwrYeVddRDzApSvlQCRAj4U5qb96PEVPQAAAwAAAAEAAQAAAQAAAAABAAAAgAAAAKIclEazlmjfS+Ic/n2MejrrDgeLWOR+bO5sk3lt1C55ON0iTwLcuFvJO5SLQdbtyPd/MmBz3cY/obMnpsW5xRvVvT+HLZXKR0AnSF689shMfObMUfLHNmHBXxRYpP8yJwvaiPWtrRQsHCth5V11EPMClK+VAJECPhTmpv3o8RU9AAADAAAAAQABAACAAAAARTaohAs3A62ay36PlXD92tvy0HvJRz1HKwDGckjXF8R/58I3M8SjHeUr//747Ff3vtva2V1+6Z7PXhtm0/K92bd+7nUg5LnhrJkUrBg72oJEbm3D3JnhIpH426i3qB6ikwgWJc6iXRxWhmCRrySDqI6Y7UilLXYJNBuEB1qobu0AAEAAAADR0SJ++4ulnyHewXCFKBwqOfdI7T/SEK/zvCmpZ0psJzHPYNwUPesimf2fM1HT9pUYoJtX/HxwYMlOnlMIFV9HAABAAAAAxctS1k1qOo5QRFM+8Olj2JD4g56Sm9+CCPbTgTM2sldD1AN6Lcs9I+8xRd1123d56rVYUglJAqE2ScyIKReRWwAAQAAAAINRu59yXNLKKG6872nH0ujy4ry4EOuvGhEYwZKPXVsXpOtAmj3pCxPbAYv0mNBrUqHZU6DcHB5R1WzpVtqGVd0AAEAAAAClS7sJjx60mtQnlW4nRNSFWvnkfPPGwe3Y8fVlptgSCG2N0+4aMRwCWNXTrjTdWSaQ6EZMIKhcQz+sQvqTZLe7AABAAAAAQMkWndgWi+meBmhEWTIJEERlEQmTGRzsJU8sATuVnLrmbhpmNoklCJAGjCg+fDdte9Zh7RwMAnd9XT7Ce7R/1g==",
      // format 2
      "AgACAAAAjAAAAAAAAAAwgYkCgYEA49cG1Kw+X+aAELjbCpD+YLL4P3n4FShcIIk6oyBz9sAkZr+jdEylrjZmCJV1bbvkWK/BWUC9eVcGq+2voLIe7XYvGxHR6c6AisUy1UFQtXsTxFR3JrstF7X3dum7yLTMUWURFVKvvg+m+O0lNDGKuCLdOw48GhO9qfJUDjGdhU0CAwEAAQMABQACAAUAAQACAAAAYAIAAAAAAAAwggJcAgEAAoGBAOPXBtSsPl/mgBC42wqQ/mCy+D95+BUoXCCJOqMgc/bAJGa/o3RMpa42ZgiVdW275FivwVlAvXlXBqvtr6CyHu12LxsR0enOgIrFMtVBULV7E8RUdya7LRe193bpu8i0zFFlERVSr74PpvjtJTQxirgi3TsOPBoTvanyVA4xnYVNAgMBAAECgYBa1nMpvZaMw6iRgjIKj25r/U7OeAF55gJ9EllnilvRqB+bO5KenqR7xcJ+b6BG5N+QoFoVDKO+Sf0l9PjNt9ZuE7FZsGXTIju6e2U3v8vRwx/bPg63b6Ocr6fthqhqzHifCdy037y8ighrw79t8AnLY+PybiVv72r3bR/EwUsGgQJBAPty4hkyZmUX61daK/oqENwFmQ9i1gmfJwm2iFNwgnIOS7hg7WwC8+0q/m56sql3PyljESuJPiQbjDuROyckke0CQQDn9r/TuHPhz+HB2njtSsfdFlsuQGE1cE8l39Ol3OaRt1XS0pjItUWEdcnWqmk9JacQtDD6YXDUq+A+RNlIBdThAkEA0X9vR8UJMRNTvkzftSeVMmvrkbksUYnrkZkmTjbASF0G05DWX5a6OxEGOXA7KRy0YDwtExPe1WCqXvrqyPscJQJAc5vIGWZJ4xZvhuUy9dkaaXcV9KRDjpWt7Mt0OasyAxfjn1I0sXue7Rijguil0MLuU+Xqry9XQ7dx6Lb5r+qwIQJAFcMio33/aCkaP9pDRZO/yxkAIKG7h6UCXMa4JNK9y0xWVlirzvCnGkzVonOk+DfA6Mrb22gVH5Jobuq+++nUrwMABQACAA=="
      };

    infinit::cryptography::test::formats<infinit::cryptography::rsa::KeyPair>(
      archives, std::bind(_test_operate, std::placeholders::_1));
  }
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("rsa/KeyPair");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}
