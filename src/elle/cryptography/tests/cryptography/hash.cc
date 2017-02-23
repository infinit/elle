#include "cryptography.hh"

#include <elle/cryptography/Oneway.hh>
#include <elle/cryptography/hash.hh>
#include <elle/cryptography/random.hh>

#include <elle/serialization/json.hh>

static std::string const _message(
  "- Do you think she's expecting something big?"
  "- You mean, like anal?");

/*----------.
| Represent |
`----------*/

template <uint32_t N,
          elle::cryptography::Oneway O>
void
test_represent_n()
{
  // N)
  {
    elle::Buffer digest = elle::cryptography::hash(_message, O);

    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      output.serialize("digest", digest);
    }

    elle::fprintf(std::cout, "[representation %s] %s\n", N, stream.str());
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
  test_represent_n<1, elle::cryptography::Oneway::md5>();
  // SHA.
  test_represent_n<2, elle::cryptography::Oneway::sha>();
  // SHA-1.
  test_represent_n<3, elle::cryptography::Oneway::sha1>();
  // SHA-224.
  test_represent_n<4, elle::cryptography::Oneway::sha224>();
  // SHA-256.
  test_represent_n<5, elle::cryptography::Oneway::sha256>();
  // SHA-384.
  test_represent_n<6, elle::cryptography::Oneway::sha384>();
  // SHA-512.
  test_represent_n<7, elle::cryptography::Oneway::sha512>();
}

/*--------.
| Operate |
`--------*/

static
void
test_operate()
{
  std::string data =
    elle::cryptography::random::generate<std::string>(123);

  elle::Buffer digest1 =
    elle::cryptography::hash(data,
                                elle::cryptography::Oneway::sha1);
  elle::Buffer digest2 =
    elle::cryptography::hash(data,
                                elle::cryptography::Oneway::sha1);

  BOOST_CHECK_EQUAL(digest1, digest2);
}

/*----------.
| Serialize |
`----------*/

template <elle::cryptography::Oneway O>
void
test_serialize_x(std::string const& R)
{
  std::stringstream stream(R);
  typename elle::serialization::json::SerializerIn input(stream);
  elle::Buffer digest;
  input.serialize("digest", digest);

  elle::Buffer _digest = elle::cryptography::hash(_message, O);

  BOOST_CHECK_EQUAL(digest, _digest);
}

static
void
test_serialize()
{
  // MD5 based on [representation 1].
  test_serialize_x<elle::cryptography::Oneway::md5>(R"JSON({"digest":"lRs+RbdXhAcwwOIi63EJHw=="})JSON");
  // SHA based on [representation 2].
  test_serialize_x<elle::cryptography::Oneway::sha>(R"JSON({"digest":"B34bV98RTfK7qXPQd3+zs3GBYKk="})JSON");
  // SHA-1 based on [representation 3].
  test_serialize_x<elle::cryptography::Oneway::sha1>(R"JSON({"digest":"LMHmhHUOH8N3mGo1HTRFd6vbmXk="})JSON");
  // SHA-224 based on [representation 4].
  test_serialize_x<elle::cryptography::Oneway::sha224>(R"JSON({"digest":"5DwfnlGVKvLuciDUwR6fuzOS2DDyA4nCeDoZ/Q=="})JSON");
  // SHA-256 based on [representation 5].
  test_serialize_x<elle::cryptography::Oneway::sha256>(R"JSON({"digest":"Ooj0FMtgjoI7saciFCZ/Xg8eXJWFhzXn89mZide6oeI="})JSON");
  // SHA-384 based on [representation 6].
  test_serialize_x<elle::cryptography::Oneway::sha384>(R"JSON({"digest":"rjLzR+ByZsSO+l+IeKrzH7Dre3XZmeK2/GSPW/TgTMBQ6VwzUDkgzjlqaKUOga/4"})JSON");
  // SHA-512 based on [representation 7].
  test_serialize_x<elle::cryptography::Oneway::sha512>(R"JSON({"digest":"i641WSGHk8ZjTReGfZSAaey8Ci1rbX+usbN7DLFM2hjjN1IlgJ2bDipopotlgY++PnN6dlN+Vd6MKIz3z1LUew=="})JSON");
}

/*-------.
| Update |
`-------*/

template <elle::cryptography::Oneway O>
void
test_update_x()
{
  std::string str = "123abcxyz";
  elle::Buffer all(str);
  auto all_digest = elle::cryptography::hash(all, O);
  std::vector<elle::Buffer> chunks;
  for (int i = 0; unsigned(i) < str.size() / 3; i++)
    chunks.emplace_back(elle::Buffer(str.data() + (i * 3), 3));
  auto context = elle::cryptography::hash_init(O);
  for (auto const& b: chunks)
    elle::cryptography::hash_update(&context, b);
  auto chunks_digest = elle::cryptography::hash_finalize(&context);
  BOOST_CHECK_EQUAL(all_digest, chunks_digest);
}

static
void
test_update()
{
  test_update_x<elle::cryptography::Oneway::md5>();
  test_update_x<elle::cryptography::Oneway::sha>();
  test_update_x<elle::cryptography::Oneway::sha1>();
  test_update_x<elle::cryptography::Oneway::sha224>();
  test_update_x<elle::cryptography::Oneway::sha256>();
  test_update_x<elle::cryptography::Oneway::sha384>();
  test_update_x<elle::cryptography::Oneway::sha512>();
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("hash");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_serialize));
  suite->add(BOOST_TEST_CASE(test_update));

  boost::unit_test::framework::master_test_suite().add(suite);
}
