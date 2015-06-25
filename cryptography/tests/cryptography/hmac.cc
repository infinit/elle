#include <cryptography/Exception.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/Plain.hh>
#include <cryptography/hmac.hh>
#include <cryptography/random.hh>

#include <elle/test.hh>
#include <elle/serialization/json.hh>

static std::string const _input(
  "- Back off Susan Boyle!");

/*----------.
| Represent |
`----------*/

template <elle::Natural32 N,
          infinit::cryptography::Oneway O>
void
test_represent_n(elle::String const& key)
{
  // N)
  {
    infinit::cryptography::Digest digest =
      infinit::cryptography::hmac::sign(
        infinit::cryptography::Plain(_input),
        key,
        O);

    std::stringstream stream;
    {
      typename elle::serialization::Json::SerializerOut output(stream);
      digest.serialize(output);
    }

    elle::printf("[representation %s] %s\n", N, stream.str());
  }
}

static
void
test_represent()
{
  // WARNING: To uncomment only if one wants to update the representations.
  return;

  // These generate hexadecimal-based representations which can be used in
  // other tests.

  // MD5.
  test_represent_n<1, infinit::cryptography::Oneway::md5>("one");
  // SHA.
  test_represent_n<2, infinit::cryptography::Oneway::sha>("deux");
  // SHA-1.
  test_represent_n<3, infinit::cryptography::Oneway::sha1>("san");
  // SHA-224.
  test_represent_n<4, infinit::cryptography::Oneway::sha224>("cetiri");
  // SHA-256.
  test_represent_n<5, infinit::cryptography::Oneway::sha256>("négy");
  // SHA-384.
  test_represent_n<6, infinit::cryptography::Oneway::sha384>("seis");
  // SHA-512.
  test_represent_n<7, infinit::cryptography::Oneway::sha512>("yedi");
}

/*--------.
| Operate |
`--------*/

void
test_operate()
{
  elle::String key =
    infinit::cryptography::random::generate<elle::String>(32);

  // HMAC sign/verify a random content.
  {
    elle::Buffer buffer =
      infinit::cryptography::random::generate<elle::Buffer>(49);

    infinit::cryptography::Digest digest =
      infinit::cryptography::hmac::sign(
        infinit::cryptography::Plain(buffer),
        key,
        infinit::cryptography::Oneway::sha1);

    BOOST_CHECK_EQUAL(infinit::cryptography::hmac::verify(
                        digest,
                        infinit::cryptography::Plain(buffer),
                        key,
                        infinit::cryptography::Oneway::sha1),
                      true);
  }

  // Try to verify an invalid HMAC.
  {
    elle::Buffer buffer =
      infinit::cryptography::random::generate<elle::Buffer>(16);

    BOOST_CHECK_EQUAL(
      infinit::cryptography::hmac::verify(
        infinit::cryptography::Digest(buffer),
        infinit::cryptography::Plain(_input),
        key,
        infinit::cryptography::Oneway::md5),
      false);
  }
}

/*----------.
| Serialize |
`----------*/

template <infinit::cryptography::Oneway O>
void
test_serialize_x(elle::String const& key,
                 elle::String const& R)
{
  std::stringstream stream(R);
  typename elle::serialization::Json::SerializerIn input(stream);
  infinit::cryptography::Digest digest(input);

  BOOST_CHECK_EQUAL(
    infinit::cryptography::hmac::verify(
      digest,
      infinit::cryptography::Plain(_input),
      key,
      O),
    true);
}

static
void
test_serialize()
{
  // MD5 based on [representation 1].
  test_serialize_x<infinit::cryptography::Oneway::md5>(
    "one",
    R"JSON({"buffer":"iflNwqB+X5jMsIlfrtuFlw=="})JSON");
  // SHA based on [representation 2].
  test_serialize_x<infinit::cryptography::Oneway::sha>(
    "deux",
    R"JSON({"buffer":"6czXAnjyVKDUp04YBujKb7BnHho="})JSON");
  // SHA-1 based on [representation 3].
  test_serialize_x<infinit::cryptography::Oneway::sha1>(
    "san",
    R"JSON({"buffer":"VTYr4FUw/mNZ/pgNs2MjL8Akc8g="})JSON");
  // SHA-224 based on [representation 4].
  test_serialize_x<infinit::cryptography::Oneway::sha224>(
    "cetiri",
    R"JSON({"buffer":"styt0RhWnMj3B2u7+bE0Rs1/AoWzt6ctQZEVRQ=="})JSON");
  // SHA-256 based on [representation 5].
  test_serialize_x<infinit::cryptography::Oneway::sha256>(
    "négy",
    R"JSON({"buffer":"a+rZICa62x5BAoBqInfRQ0B6mKXIJlAW4bgbY7hDaAI="})JSON");
  // SHA-384 based on [representation 6].
  test_serialize_x<infinit::cryptography::Oneway::sha384>(
    "seis",
    R"JSON({"buffer":"EMSoprWrQWgJKoyDLWB2t2Ir9UG48oNVVs/GpXZ9LELsfraI7RKXi6Dxz82FaDfZ"})JSON");
  // SHA-512 based on [representation 7].
  test_serialize_x<infinit::cryptography::Oneway::sha512>(
    "yedi",
    R"JSON({"buffer":"e7xVCxRZrBtQPJ7LFwhj72at4HNyxT+TjN8JHSe5TVgygvyNCkEf/8q78VluouQu712rh5s+xnCtwrayd86JLA=="})JSON");
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("hmac");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}
