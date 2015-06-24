#include "cryptography.hh"

#include <cryptography/Output.hh>
#include <cryptography/Input.hh>
#include <cryptography/random.hh>

#include <elle/serialization/json.hh>

/*----------.
| Represent |
`----------*/

static
void
test_represent()
{
  // WARNING: To uncomment only if one wants to update the representations.
  return;

  // These generate base64-based representations which can be used in
  // other tests.

  // 1)
  {
    infinit::cryptography::Output output(
      infinit::cryptography::random::generate<elle::Buffer>(128));
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << output;
    elle::printf("[representation 1] %s\n", archive);
  }
}

/*----------.
| Construct |
`----------*/

static
void
test_construct()
{
  // By size.
  infinit::cryptography::Output output1(64);

  BOOST_CHECK_EQUAL(output1.buffer().size(), 64);

  // By buffer.
  infinit::cryptography::Output output2(
    infinit::cryptography::random::generate<elle::Buffer>(128));

  // Output copy.
  infinit::cryptography::Output output3(output2);

  BOOST_CHECK_EQUAL(output2, output3);

  auto address3 = output3.buffer().contents();

  // Output move.
  infinit::cryptography::Output output4(std::move(output3));

  BOOST_CHECK_EQUAL(output2, output4);

  // Buffer copy.
  infinit::cryptography::Output output5(output4.buffer());

  BOOST_CHECK_EQUAL(output2, output4);
  BOOST_CHECK_EQUAL(output2, output5);

  // Buffer move.
  infinit::cryptography::Output output6(std::move(output4.buffer()));

  BOOST_CHECK_EQUAL(output2, output5);
  BOOST_CHECK_EQUAL(output2, output6);
  BOOST_CHECK_EQUAL(output5, output6);

  auto address6 = output6.buffer().contents();

  BOOST_CHECK_EQUAL(address3, address6);
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
    infinit::cryptography::Output output1(
      infinit::cryptography::random::generate<elle::Buffer>(128));

    elle::String archive;
    elle::serialize::to_string(archive) << output1;

    auto extractor = elle::serialize::from_string(archive);
    infinit::cryptography::Output output2(extractor);

    BOOST_CHECK_EQUAL(output1, output2);
  }

  // Deserialize from the hard-coded string [representation 1]: useful
  // for detecting changes in formats.
  {
    elle::String archive1("AAAAAIAAAAAAAAAAYVS/sqT6qs+lo+MdcB1jaZY8gwFx6a10mr1+MjW8to4nvK1hjewkupo66RFvioIZ59hITeuy4CvfQNJSqvdVJ4xLEyWKED9oeuji8jxe1nGOt068HlVyFJ9RkZT4cN6WC8RJXekhokXjI4yr89ad0Vw8KaB5mw5U79CUKaxBVnU=");

    auto extractor1 =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive1);
    infinit::cryptography::Output output1(extractor1);

    elle::String archive2;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive2) << output1;

    auto extractor2 =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive2);
    infinit::cryptography::Output output2(extractor2);

    BOOST_CHECK_EQUAL(output1, output2);
  }
}

/*--------.
| Compare |
`--------*/

static
void
test_compare()
{
  infinit::cryptography::Output output1(
    infinit::cryptography::random::generate<elle::Buffer>(4096));

  // Output/output.
  infinit::cryptography::Output output2(
    infinit::cryptography::random::generate<elle::Buffer>(4096));

  // With high probabilituy, this should not be the case. Otherwise,
  // the random generator is probably broken.
  BOOST_CHECK(output1 != output2);
  BOOST_CHECK(!(output1 == output2));

  if (output1 < output2)
  {
    BOOST_CHECK(output1 <= output2);
    BOOST_CHECK(!(output1 > output2));
    BOOST_CHECK(!(output1 >= output2));
  }
  else
  {
    BOOST_CHECK(output1 >= output2);
    BOOST_CHECK(!(output1 < output2));
    BOOST_CHECK(!(output1 <= output2));
  }

  // Output/input.
  infinit::cryptography::Input input2(output2.buffer());

  BOOST_CHECK(output1 != input2);
  BOOST_CHECK(!(output1 == input2));

  if (output1 < input2)
  {
    BOOST_CHECK(output1 <= input2);
    BOOST_CHECK(!(output1 > input2));
    BOOST_CHECK(!(output1 >= input2));
  }
  else
  {
    BOOST_CHECK(output1 >= input2);
    BOOST_CHECK(!(output1 < input2));
    BOOST_CHECK(!(output1 <= input2));
  }
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("Output");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_serialize));
  suite->add(BOOST_TEST_CASE(test_compare));

  boost::unit_test::framework::master_test_suite().add(suite);
}
