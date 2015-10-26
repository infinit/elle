#include "../cryptography.hh"

#include <cryptography/dsa/PrivateKey.hh>
#include <cryptography/dsa/PublicKey.hh>
#include <cryptography/dsa/KeyPair.hh>

#include <elle/printf.hh>
#include <elle/serialization/json.hh>

/*----------.
| Represent |
`----------*/

static
void
_test_represent()
{
  infinit::cryptography::dsa::KeyPair keypair =
    infinit::cryptography::dsa::keypair::generate(1024);

  // 1)
  {
    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      infinit::cryptography::dsa::PrivateKey k = keypair.k();
      k.serialize(output);
    }

    elle::printf("[representation 1] %s\n", stream.str());
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

  _test_represent();
}

/*---------.
| Generate |
`---------*/

static
infinit::cryptography::dsa::PrivateKey
_test_generate(uint32_t const length = 1024)
{
  infinit::cryptography::dsa::KeyPair keypair =
    infinit::cryptography::dsa::keypair::generate(length);

  infinit::cryptography::dsa::PrivateKey k(keypair.k());

  return (k);
}

static
void
test_generate()
{
  _test_generate();
}

/*----------.
| Construct |
`----------*/

static
void
test_construct()
{
  infinit::cryptography::dsa::PrivateKey k1 = _test_generate(2048);

  // PrivateKey copy.
  infinit::cryptography::dsa::PrivateKey k2(k1);

  BOOST_CHECK_EQUAL(k1, k2);

  // PrivateKey move.
  infinit::cryptography::dsa::PrivateKey k3(std::move(k1));

  BOOST_CHECK_EQUAL(k2, k3);
}

/*--------.
| Compare |
`--------*/

static
void
test_compare()
{
  infinit::cryptography::dsa::PrivateKey k1 = _test_generate(1024);
  infinit::cryptography::dsa::PrivateKey k2 = _test_generate(1024);

  // With high probabilituy, this should not be the case. Otherwise,
  // the random generator is probably broken.
  BOOST_CHECK(k1 != k2);
  BOOST_CHECK(!(k1 == k2));
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
    infinit::cryptography::dsa::PrivateKey k1 = _test_generate(2048);

    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      k1.serialize(output);
    }

    typename elle::serialization::json::SerializerIn input(stream);
    infinit::cryptography::dsa::PrivateKey k2(input);

    BOOST_CHECK_EQUAL(k1, k2);
  }

  // For every hard-coded strings [representation 1] in every format,
  // deserialize the public keys, re-serialize them, make sure they can
  // be used and finally upgrade them: useful for detecting changes in formats.
  {
    std::vector<std::string> const archives{
      // format 0
      R"JSON({"digest algorithm":4,"dsa":"MIIBuwIBAAKBgQDTqOWZ7rSD+2LLZAae5TnEH576/xpwKNWU0KJ1bw1vrFO6i1zlxstE0JLl++tkWFuklbU5an3oGqOhfw0ClryxBWOfMVVPQ3+MWEvn/heU9q+1odOaTrm985KZp6aqmHRqXnLGWnVN5yuO/JwwzS/Gz6mtPMIq00/C2RjvVkeCYwIVAJ9b7Z4C5+cy9dxkRHIObd2jqpX5AoGBAIhF3LRrhlkD37PGzirFmVxkg/mz1r1FNHDNqzUFJpTey8JdUEvyKecx+H2yI6YLM37BjzujWWn75J37ZQ3EuA1mmQ08zdUJKEyiKh297Nibx/rnYVmhFol1OAl5LVn18IYICALVfpTvusiNlPaJ5ai2SjALEBHfPfyGjqPDfAqGAoGAet+hjm07E01WdO76jF1SHAIcbU2WYkjNqovmiHuxd6AZcjQU8r7EFCRVG6Chge89pXVqr7vqfci5VY44LUh4gA1opIx3dIggL0W66DJUvrDmGMvshwRL0vw+/nHtIRvbrmfad443Ghdl5wqLC3Mg0C6DmkKkY6vd2asfjiqQjjQCFC8088Cg7wtkCCZ6MfHlFd679Vyy"})JSON"
    };

    infinit::cryptography::test::formats<
      infinit::cryptography::dsa::PrivateKey>(archives);
  }
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("dsa/PrivateyKey");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_compare));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}
