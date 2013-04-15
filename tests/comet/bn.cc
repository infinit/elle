#include "comet.hh"

#include <comet/bn.hh>

/*--------.
| Operate |
`--------*/

void
test_operate_rsa()
{
  /* XXX
  infinit::cryptography::KeyPair pair = test_generate_rsa(1024);

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
    Sample const input(
      42, infinit::cryptography::random::generate<elle::String>(14920));
    infinit::cryptography::Code code = pair.K().encrypt(input);
    Sample const output = pair.k().decrypt<Sample>(code);

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
    Sample const input(
      84, infinit::cryptography::random::generate<elle::String>(28130));
    infinit::cryptography::Code code = pair.k().encrypt(input);
    Sample const output = pair.K().decrypt<Sample>(code);

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
    auto result =
      pair.K().verify(signature,
                      infinit::cryptography::Plain{
                        elle::WeakBuffer{reinterpret_cast<void*>(const_cast<char*>(input.c_str())),
                                         input.length()}});

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
  */
}

void
test_operate()
{
  // RSA.
  test_operate_rsa();
}

/*-----.
| Main |
`-----*/

bool
test()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("bn");

  suite->add(BOOST_TEST_CASE(test_operate));

  boost::unit_test::framework::master_test_suite().add(suite);

  return (true);
}

int
main(int argc,
     char** argv)
{
  return (boost::unit_test::unit_test_main(test, argc, argv));
}
