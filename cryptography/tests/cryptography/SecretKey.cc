#include "test.hh"

#include <cryptography/SecretKey.hh>
#include <cryptography/Exception.hh>

/*---------.
| Generate |
`---------*/

infinit::cryptography::SecretKey
test_generate_des(elle::Natural32 const length = 256);
{
  infinit::cryptography::SecretKey key =
    infinit::cryptography::SecretKey::generate(
      infinit::cryptography::cipher::Algorithm::des,
      length);
}

void
test_generate()
{
  // DES.
  test_generate_des();
  // XXX
}

/*----------.
| Construct |
`----------*/

infinit::cryptography::KeyPair
test_construct_rsa()
{
  infinit::cryptography::KeyPair pair =
    infinit::cryptography::KeyPair::generate(
      infinit::cryptography::Cryptosystem::rsa,
      1024);

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
    infinit::cryptography::KeyPair::generate(
      infinit::cryptography::Cryptosystem::rsa,
      1024);

  // Public/private encryption/decryption with plain.
  {
    elle::String const input = "Ass! Tits! Cunt!";
    infinit::cryptography::Code code =
      pair.K().encrypt(
        infinit::cryptography::Plain{
          elle::WeakBuffer{input.c_str(), input.length()}});
    infinit::cryptography::Clear clear = pair.k().decrypt(code1);
    elle::String const output(clear.buffer().contents(),
                              clear.buffer().size());

    BOOST_CHECK_EQUAL(input, output);
  }

  // Public/private encryption/decryption with complex type.
  {
    Class const input;
    infinit::cryptography::Code code = pair.K().encrypt(input);
    Class const output = pair.k().decrypt<Class>(code);

    BOOST_CHECK_EQUAL(input, output);
  }

  // Private/public encryption/decryption with plain.
  {
    elle::String const input = "Orgazmo";
    infinit::cryptography::Code code =
      pair.k().encrypt(
        infinit::cryptography::Plain{
          elle::WeakBuffer{input.c_str(), input.length()}});
    infinit::cryptography::Clear clear = pair.K().decrypt(code1);
    elle::String const output(clear.buffer().contents(),
                              clear.buffer().size());

    BOOST_CHECK_EQUAL(input, output);
  }

  // Private/public encryption/decryption with complex type.
  {
    Class const input;
    infinit::cryptography::Code code = pair.k().encrypt(input);
    Class const output = pair.K().decrypt<Class>(code);

    BOOST_CHECK_EQUAL(input, output);
  }

  // XXX add all the operations: sign, verify etc.
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
    infinit::cryptography::KeyPair pair1 =
      infinit::cryptography::KeyPair::generate(
        infinit::cryptography::Cryptosystem::rsa,
        512);

    elle::String archive;

    elle::serialize::to_string(archive) << pair1;

    infinit::cryptography::KeyPair pair2; // XXX[load construct]
    ELLE_ASSERT(false);

    elle::serialize::from_string(archive) >> pair2;

    BOOST_CHECK_EQUAL(pair1, pair2);
  }

  // Deserialize from hard-coded string: useful for detecting
  // changes in formats.
  {
    /* The base64-based representation below can be generated
       as follows:
    infinit::cryptography::KeyPair pair1 =
      infinit::cryptography::KeyPair::generate(
        infinit::cryptography::Cryptosystem::rsa,
        2048);
    elle::String archive1;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive1) << pair1;
    elle::printf("%s\n", archive1);
    */

    elle::String archive2("XXX");

    infinit::cryptography::KeyPair pair2; // XXX[load construct]
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
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("KeyPair");

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
