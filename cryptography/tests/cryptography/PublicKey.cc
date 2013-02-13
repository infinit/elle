#include "test.hh"

#include <cryptography/PublicKey.hh>
#include <cryptography/PrivateKey.hh>
#include <cryptography/Exception.hh>
#include <cryptography/rsa/keypair.hh>

/*---------.
| Generate |
`---------*/

infinit::cryptography::PublicKey
test_generate_rsa(elle::Natural32 const length = 1024)
{
  // By implementation.
  std::pair<infinit::cryptography::rsa::PublicKey,
            infinit::cryptography::rsa::PublicKey> pair =
    infinit::cryptography::rsa::keypair::generate(length);

  std::unique_ptr<infinit::cryptography::publickey::Interface> interface(
    new rsa::PublicKey{std::move(pair.second)});

  infinit::cryptography::PublicKey k(std::move(inteface));

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
  infinit::cryptography::PublicKey k =
    test_generate_rsa(2048);

  // PublicKey copy.
  infinit::cryptography::PublicKey k2(k1);

  BOOST_CHECK_EQUAL(k1, k2);

  // PublicKey move.
  infinit::cryptography::PublicKey k3(std::move(k1));

  BOOST_CHECK_EQUAL(k2, k3);
}

/*--------.
| Operate |
`--------*/

void
test_operate_rsa()
{
  infinit::cryptography::PublicKey k =
    test_generate_rsa(512);

  // The following operations are based on hard-coded base64 string which
  // represent the data on which to operate, in their serialized form.

  // Decrypt plain.
  {
    elle::String const input1("my balls are ok!");
    /* The base64-based representation below can be generated
       as follows:
    infinit::cryptography::KeyPair pair1 =
      infinit::cryptography::KeyPair::generate(
        infinit::cryptography::Cryptosystem::rsa,
        2048);
    infinit::cryptography::Code code1 =
      pair.K().encrypt(
        infinit::cryptography::Plain(
          elle::WeakBuffer{reinterpret_cast<void*>(const_cast<char*>(input1.c_str())),
                                                   input1.length()}));
    elle::String archive1;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive1) << code1;
    elle::printf("%s\n", archive1);
    */

    elle::String archive2("XXX");

    infinit::cryptography::Code code2; // XXX[load construct]
    ELLE_ASSERT(false);

    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(archive2) >> code2;

    infinit::cryptography::Clear clear2 = k.decrypt(code2);
    elle::String const output2(clear2.buffer().contents(),
                               clear2.buffer().size());

    BOOST_CHECK_EQUAL(input1, output2);
  }

  // Decrypt complex type.
  {
    Class input1("chodaboy", 23293083121);
    /* The base64-based representation below can be generated
       as follows:
    infinit::cryptography::KeyPair pair1 =
      infinit::cryptography::KeyPair::generate(
        infinit::cryptography::Cryptosystem::rsa,
        2048);
    infinit::cryptography::Code code1 = pair.K().encrypt(input1);
    elle::String archive1;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive1) << code1;
    elle::printf("%s\n", archive1);
    */

    elle::String archive2("XXX");

    infinit::cryptography::Code code2; // XXX[load construct]
    ELLE_ASSERT(false);

    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(archive2) >> code2;

    Class output2 = k.decrypt<Class>(code2);

    BOOST_CHECK_EQUAL(input1, output2);
  }

  // Sign plain.
  {
    // XXX if signature is deterministic, write a test which should result with a specific base64 Signature.
    // XXX do it with both a plain and complex type
  }

  // Encrypt plain.
  {
    // XXX same for encryption (but most likely not deterministic)
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
  infinit::cryptography::PublicKey k1 =
    test_generate_rsa(1024);

  infinit::cryptography::PublicKey k2 =
    test_generate_rsa(1024);

  // With high probabilituy, this should not be the case. Otherwise,
  // the random generator is probably broken.
  BOOST_CHECK(k1 != k2);
  BOOST_CHECK(!(k1 == k2));

  if (k1 < k2)
    {
      BOOST_CEHCK(k1 <= k2);
      BOOST_CHECK(!(k1 > k2));
      BOOST_CHECK(!(k1 >= k2));
    }
  else
    {
      BOOST_CEHCK(k1 >= k2);
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
    infinit::cryptography::PublicKey k1 =
      test_generate_rsa(2048);

    elle::String archive;

    elle::serialize::to_string(archive) << k1;

    infinit::cryptography::PublicKey k2; // XXX[load construct]
    ELLE_ASSERT(false);

    elle::serialize::from_string(archive) >> k2;

    BOOST_CHECK_EQUAL(k1, k2);
  }

  // Deserialize from hard-coded string: useful for detecting
  // changes in formats.
  {
    /* The base64-based representation below can be generated
       as follows:
    infinit::cryptography::PublicKey k1 =
      test_generate_rsa(1024);
    elle::String archive1;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive1) << pair1;
    elle::printf("%s\n", archive1);
    */

    elle::String archive2("XXX");

    infinit::cryptography::PublicKey k2; // XXX[load construct]
    ELLE_ASSERT(false);

    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(archive2) >> pair2;

    // XXX re-serialize and compare
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
  return boost::unit_test::unit_test_main(test, argc, argv);
}
