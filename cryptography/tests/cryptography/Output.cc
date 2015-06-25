#include <cryptography/Output.hh>
#include <cryptography/Input.hh>
#include <cryptography/random.hh>

#include <elle/test.hh>
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
    infinit::cryptography::Output object(
      infinit::cryptography::random::generate<elle::Buffer>(128));

    std::stringstream stream;
    {
      typename elle::serialization::Json::SerializerOut output(stream);
      object.serialize(output);
    }

    elle::printf("[representation 1] %s\n", stream.str());
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
  infinit::cryptography::Output object1(64);

  BOOST_CHECK_EQUAL(object1.buffer().size(), 64);

  // By buffer.
  infinit::cryptography::Output object2(
    infinit::cryptography::random::generate<elle::Buffer>(128));

  // Output copy.
  infinit::cryptography::Output object3(object2);

  BOOST_CHECK_EQUAL(object2, object3);

  auto address3 = object3.buffer().contents();

  // Output move.
  infinit::cryptography::Output object4(std::move(object3));

  BOOST_CHECK_EQUAL(object2, object4);

  // Buffer copy.
  infinit::cryptography::Output object5(object4.buffer());

  BOOST_CHECK_EQUAL(object2, object4);
  BOOST_CHECK_EQUAL(object2, object5);

  // Buffer move.
  infinit::cryptography::Output object6(std::move(object4.buffer()));

  BOOST_CHECK_EQUAL(object2, object5);
  BOOST_CHECK_EQUAL(object2, object6);
  BOOST_CHECK_EQUAL(object5, object6);

  auto address6 = object6.buffer().contents();

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
    infinit::cryptography::Output object1(
      infinit::cryptography::random::generate<elle::Buffer>(128));

    std::stringstream stream1;
    {
      typename elle::serialization::Json::SerializerOut output(stream1);
      object1.serialize(output);
    }

    std::stringstream stream2(stream1.str());
    typename elle::serialization::Json::SerializerIn input(stream2);
    infinit::cryptography::Output object2(input);

    BOOST_CHECK_EQUAL(object1, object2);
  }

  // Deserialize from the hard-coded string [representation 1]: useful
  // for detecting changes in formats.
  {
    elle::String archive1(R"JSON({"buffer":"sxR6x3SeZLaNrXnJ1a4bvhL0vw0II76mYG7Siyb9Vjz5PFGIFQQAKhB6wqltt73fI0ydU5lksG7VBKuyJprMYdWhjQ9JIJf/4Otf+CmtPCnlSjGD90wqUdGVjvjDBK7oPjIiKvKLg5ZerpNWB0PEfQTAuj31bqIAbsRe0A6kWBQ="})JSON");

    std::stringstream stream1(archive1);
    typename elle::serialization::Json::SerializerIn input1(stream1);
    infinit::cryptography::Output object1(input1);

    std::stringstream stream2;
    {
      typename elle::serialization::Json::SerializerOut output2(stream2);
      object1.serialize(output2);
    }

    typename elle::serialization::Json::SerializerIn input2(stream2);
    infinit::cryptography::Output object2(input2);

    BOOST_CHECK_EQUAL(object1, object2);
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
