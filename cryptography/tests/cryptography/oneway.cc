#include "cryptography.hh"
#include "Sample.hh"

#include <cryptography/Exception.hh>
#include <cryptography/oneway.hh>
#include <cryptography/random.hh>

#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

/*--------.
| Operate |
`--------*/

template <infinit::cryptography::oneway::Algorithm A,
          elle::Natural32 S>
void
test_operate_x()
{
  // Hash a plain.
  {
    elle::String const input =
      "- Do you think she's expecting something big?"
      "- You mean, like anal?";

    infinit::cryptography::Digest digest1 =
      infinit::cryptography::oneway::hash(
        infinit::cryptography::Plain(
          elle::WeakBuffer(reinterpret_cast<void*>(const_cast<char*>(input.c_str())),
                           input.length())),
        A);
    infinit::cryptography::Digest digest2 =
      infinit::cryptography::oneway::hash(
        infinit::cryptography::Plain(
          elle::WeakBuffer(reinterpret_cast<void*>(const_cast<char*>(input.c_str())),
                           input.length())),
        A);

    BOOST_CHECK_EQUAL(digest1, digest2);
  }

  // Hash a complex type.
  {
    Sample const input(
      42, infinit::cryptography::random::generate<elle::String>(S));

    infinit::cryptography::Digest digest1 =
      infinit::cryptography::oneway::hash(input, A);
    infinit::cryptography::Digest digest2 =
      infinit::cryptography::oneway::hash(input, A);

    BOOST_CHECK_EQUAL(digest1, digest2);
  }
}

void
test_operate()
{
  // MD5.
  test_operate_x<infinit::cryptography::oneway::Algorithm::md5, 820>();
  // SHA.
  test_operate_x<infinit::cryptography::oneway::Algorithm::sha, 31>();
  // SHA-1.
  test_operate_x<infinit::cryptography::oneway::Algorithm::sha1, 9028>();
  // SHA-224.
  test_operate_x<infinit::cryptography::oneway::Algorithm::sha224, 630>();
  // SHA-256.
  test_operate_x<infinit::cryptography::oneway::Algorithm::sha256, 73>();
  // SHA-384.
  test_operate_x<infinit::cryptography::oneway::Algorithm::sha384, 933>();
  // SHA-512.
  test_operate_x<infinit::cryptography::oneway::Algorithm::sha512, 9223>();
}

/*-----.
| Main |
`-----*/

bool
test()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("SecretKey");

  suite->add(BOOST_TEST_CASE(test_operate));

  boost::unit_test::framework::master_test_suite().add(suite);

  return (true);
}

int
main(int argc,
     char** argv)
{
  return boost::unit_test::unit_test_main(test, argc, argv);
}
