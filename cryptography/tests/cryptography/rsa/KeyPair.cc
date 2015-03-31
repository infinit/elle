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
      "AAAAAAAAjAAAAAAAAAAwgYkCgYEA2a8vxywsYs4nGp2ri6S4xQ8/A6TDE77I9sz3uhRvJPYyY1zpFZFqBt2FMEJl48CXJOPVVHnW4MwLRL+NgRpJ22v9cY2nn80jVkKsxWHvTr1NX0HfCohncBMjZ2+YL8Sdo2Mao5aoYE1nzPJw1CE3mjWasz68/HmKnVHQLRGc6y0CAwEAAQMABQACAAUAAQAAAAAAYAIAAAAAAAAwggJcAgEAAoGBANmvL8csLGLOJxqdq4ukuMUPPwOkwxO+yPbM97oUbyT2MmNc6RWRagbdhTBCZePAlyTj1VR51uDMC0S/jYEaSdtr/XGNp5/NI1ZCrMVh7069TV9B3wqIZ3ATI2dvmC/EnaNjGqOWqGBNZ8zycNQhN5o1mrM+vPx5ip1R0C0RnOstAgMBAAECgYAkiXsDey6YBvIZTY2VirhYmw6Alg8IKWWhxxVtbjCZi5k5hEa7EAUz0cqhJwPjDlhQIM2VuD2Qza62g1BE8xyiIFkvsaoRAa+0+LPTpdKYdD+FNhkmoLdYF11JSjX+ptxi4YhCF3FCYGnqbi+9OeCgLTPalygFKv2rnTJlT26gIQJBAPT8eVuESDZCzMUc29IQyr2rZN0koAVmXeRUTaN6HhcWDjkzyCGGuU+MK4jtPOfSaexTRXi20gqtUDxvxXYW4qcCQQDjeH9MKZwz/3l4JPFX3o5D9kjU2QaswZBxtZQhZ8bn64WSpir20nuShG5lXha4m4GDFnfB3gk4/epXJSP7ISILAkEAtEOejJ+OfxayFAxDF5QxilS2WB6d/LsxNVRu+G5f+77H4oFeroSykHzkf9GI8k+aj0FKqiNj9GVI19+qnVUDmwJALat1s27vQ6lZU/vm0MOIQLDj8ogYRPsf6xSPxx003D93M/hUy/V6hfI8z/FSEoJ2QEF0jeHsbBrev9xfGv6N/QJATyXzQPeQYvkivJCczyZZ9k/lKHbPlR4K/ntqhWpqdArUib7cD62qQ9pEi3/p8XxAqgTOu0BlR4VWD3+3KL+bdQMABQACAA=="
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
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("KeyPair");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}
