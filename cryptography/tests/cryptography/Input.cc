#include <elle/test.hh>

#include <cryptography/Input.hh>
#include <cryptography/Output.hh>
#include <cryptography/random.hh>

/*-------------.
| Construction |
`-------------*/

static
void
test_construction()
{
  // Implicit construction from ConstWeakBuffer.
  {
    elle::Natural32 size = 256;
    elle::Buffer buffer =
      infinit::cryptography::random::generate<elle::Buffer>(size);
    elle::ConstWeakBuffer constweakbuffer(buffer);

    infinit::cryptography::Input input(constweakbuffer);

    BOOST_CHECK_EQUAL(input.buffer().size(), size);
    BOOST_CHECK_EQUAL(input.buffer(), constweakbuffer);

    BOOST_CHECK_EQUAL(input.buffer().size(), size);
    BOOST_CHECK_EQUAL(input.buffer(), buffer);
  }

  // Implicit construction from WeakBuffer.
  {
    elle::Natural32 size = 64;
    elle::Buffer buffer =
      infinit::cryptography::random::generate<elle::Buffer>(size);
    elle::WeakBuffer weakbuffer(buffer);

    infinit::cryptography::Input input(weakbuffer);

    BOOST_CHECK_EQUAL(input.buffer().size(), size);
    BOOST_CHECK_EQUAL(input.buffer(), weakbuffer);

    BOOST_CHECK_EQUAL(input.buffer().size(), size);
    BOOST_CHECK_EQUAL(input.buffer(), buffer);
  }

  // Implicit construction from Buffer.
  {
    elle::Natural32 size = 512;
    elle::Buffer buffer =
      infinit::cryptography::random::generate<elle::Buffer>(size);

    infinit::cryptography::Input input(buffer);

    BOOST_CHECK_EQUAL(input.buffer().size(), size);
    BOOST_CHECK_EQUAL(input.buffer(), buffer);
  }
}

/*-----------.
| Comparison |
`-----------*/

static
void
test_comparison()
{
  elle::Buffer buffer1 =
    infinit::cryptography::random::generate<elle::Buffer>(4096);
  elle::Buffer buffer2 =
    infinit::cryptography::random::generate<elle::Buffer>(4096);

  infinit::cryptography::Input input1(buffer1);
  infinit::cryptography::Input input2(buffer2);

  // With high probabilituy, this should not be the case. Otherwise,
  // the random generator is probably broken.
  BOOST_CHECK(input1 != input2);
  BOOST_CHECK(!(input1 == input2));

  if (input1 < input2)
  {
    BOOST_CHECK(input1 <= input2);
    BOOST_CHECK(!(input1 > input2));
    BOOST_CHECK(!(input1 >= input2));
  }
  else
  {
    BOOST_CHECK(input1 >= input2);
    BOOST_CHECK(!(input1 < input2));
    BOOST_CHECK(!(input1 <= input2));
  }

  // Input/output.
  infinit::cryptography::Output output2(buffer2);

  BOOST_CHECK(input1 != output2);
  BOOST_CHECK(!(input1 == output2));

  if (input1 < output2)
  {
    BOOST_CHECK(input1 <= output2);
    BOOST_CHECK(!(input1 > output2));
    BOOST_CHECK(!(input1 >= output2));
  }
  else
  {
    BOOST_CHECK(input1 >= output2);
    BOOST_CHECK(!(input1 < output2));
    BOOST_CHECK(!(input1 <= output2));
  }
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("Input");

  suite->add(BOOST_TEST_CASE(test_construction));
  suite->add(BOOST_TEST_CASE(test_comparison));

  boost::unit_test::framework::master_test_suite().add(suite);
}
