#include "test.hh"

#include <cryptography/KeyPair.hh>
#include <cryptography/PublicKey.hh>
#include <cryptography/PrivateKey.hh>
#include <cryptography/Exception.hh>

#include <elle/types.hh>
#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

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
    elle::String const input = "Ass! Tits! Cunt!";
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
    Class const input(42, "chuck");
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
          elle::WeakBuffer{reinterpret_cast<void*>(const_cast<char*>(input.c_str())),
                                                   input.length()}});
    infinit::cryptography::Clear clear = pair.K().decrypt(code);
    elle::String const output(reinterpret_cast<char const*>(clear.buffer().contents()),
                              clear.buffer().size());

    BOOST_CHECK_EQUAL(input, output);
  }

  // Private/public encryption/decryption with complex type.
  {
    Class const input(84, "norris");
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
      test_generate_rsa(512);

    elle::String archive;

    elle::serialize::to_string(archive) << pair1;

    auto extractor = elle::serialize::from_string(archive);

    infinit::cryptography::KeyPair pair2(extractor);

    BOOST_CHECK_EQUAL(pair1, pair2);
  }

  // Deserialize from hard-coded string: useful for detecting
  // changes in formats.
  {
    /* The base64-based representation below can be generated
       as follows:
    infinit::cryptography::KeyPair pair1 =
      test_generate_rsa(2048);
    elle::String archive1;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive1) << pair1;
    elle::printf("%s:%s %s\n", __PRETTY_FUNCTION__, __LINE__, archive1);
    */

    elle::String archive2("AAAAAAAAAAAAAAABAACxsL4HJA1rH/28C1Yf0/o0MCI2iLJMMy+V04PoKG8yhBIqN+wfoRvIPS/Y6zl51HRaa7nGZH+PeqK4Ja2R6g37/Bg7zLtkAS2Zy7YBXiOvq9vGYQW2fPKYQA+rhd2TZE9T9F2FJxpLBltkGQQhc18zWX2jFM/G2uRnCbnjTPUcldTRvlahCdWFDcjonMGU8BAMDZvO1BNu0eBfRRw9MnbP1cQuWEidbsqpV4QWRgb1+8ZlcYdFDOd5p3EKz3WSN2OYFD/fM9cc5JhcoNaG4oUS99P/otk0Xs8IGfbSwKB7a0eE28ELBN0mI2nSVZHA1XvevOfhxVeJfaCgO7hZxtKdAAADAAAAAQABAAAAAAAAAAAAAQAAsbC+ByQNax/9vAtWH9P6NDAiNoiyTDMvldOD6ChvMoQSKjfsH6EbyD0v2Os5edR0Wmu5xmR/j3qiuCWtkeoN+/wYO8y7ZAEtmcu2AV4jr6vbxmEFtnzymEAPq4Xdk2RPU/RdhScaSwZbZBkEIXNfM1l9oxTPxtrkZwm540z1HJXU0b5WoQnVhQ3I6JzBlPAQDA2bztQTbtHgX0UcPTJ2z9XELlhInW7KqVeEFkYG9fvGZXGHRQzneadxCs91kjdjmBQ/3zPXHOSYXKDWhuKFEvfT/6LZNF7PCBn20sCge2tHhNvBCwTdJiNp0lWRwNV73rzn4cVXiX2goDu4WcbSnQAAAwAAAAEAAQAAAAEAAEtCntoLbRuIa0HwU0Qb1oewM22BrHMV4vvEoMP+DEsLIV58XShwZoIDWHa5+pgdYkLCCed7vYOZ3727R67bwZyPA/ygFGahER1o6OffZaIMWoPFqFPIzD+sMnblthmtjOP0RoAVNc+9G4xkBBne8d2f89TVBcQmBkOWHQOnHtJB9zikoHaYL/95Xc1qf5YtjrwuYk15K6rZODKmIthPzxG8NJUQNmIP+KzquRI8/O55+6KCgj9FkpFw0puRAL3/T4cQtHd1EYxYy3n2I0BG742zJ8keIzil7HBR8OojCiGeEln5sDOC2UJOeiFfycLKkiGC4j2bocLzdaco1MP3fIEAAIAAAADX3gHvKTcA5ME0Os11y5waLC0iGRTti8PlSqVnH5Vwk0exmsbqbpeIK38yprFSsJ86myu52JFJGATNL3prKMipEEjHh8I/dUx9x8bfaVTW0ngOhWHJX7MyDF1ik/jr46p7+wk5SmNdh2uUPw7rBBLsZ2JrQW2MTKrqI48vzgkgjQAAgAAAANK5vvB8VixDGgw7qyJjqpID1aTH2KDvKjXr97t5oU8w9a0K7/pka7MJHH3OVT38vA3OKcu12/o/S8zIbfULDF14pjJgsGOIu3b8DjWMY3YZXtVIUunhwX9gbNRSqhSAXODJZwMQyKHJxX+C/Vu77UoOclay2Db4YWFgHc4+Hx5RAACAAAAAORXRpyBcFlVoLlq6iMMedeFm0hSV7ze9xz4VPY8hotKf6/zFS6uRUQZHrepdDEP+562vQ4dc7Fp4spYypnHzlpni1Tk2ijeH0d9p3RzqvaCM9TIaqPkw1zRj4bTF6AOdywmY59ifLFWqglqmpEnAg1nXsgtsX+TKcf6TWA4BZeUAAIAAAABOB9IYozIVudNPbFj2pwlmgF0AY7BoB6gNx484sNV2yZ5vsXkjs0PeTCVswJb4GTpZslnvwSvwArhC4IU3FBxEEIUaaotmkB869CMe2RmLheGEAQAS8zH/xOkHxoSMiuvZoTqv++sPXfPwP8UpzvETVz4ntJoz4M3OjRT8ZiyHwQAAgAAAANdjt/awau+zU4EEYCLgIJ5aBHEkQykDxMorjutnm+aGyu8N5rZqjS5zu3qn/JYYCrD81PPJPn1DgO/7UwWogqyjSNs/kAucqRqzixdNSg10z7AvJRwJfAsKowcvOkA7NqMW/eeZ5ryuVsBIROnCHU54/vdAflNrjXjFl0glCnYM");

    auto extractor =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive2);

    infinit::cryptography::KeyPair pair2(extractor);

    elle::String archive3;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive3) << pair2;

    BOOST_CHECK_EQUAL(archive2, archive3);
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
