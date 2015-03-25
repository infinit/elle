#include "cryptography.hh"
#include "Sample.hh"

#include <cryptography/SecretKey.hh>
#include <cryptography/Exception.hh>
#include <cryptography/random.hh>

#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

/*----------.
| Represent |
`----------*/

template <elle::Natural32 N,
          infinit::cryptography::cipher::Algorithm A,
          elle::Natural32 L>
void
test_represent_n()
{
  infinit::cryptography::SecretKey key =
    infinit::cryptography::SecretKey::generate(A, L);

  // N)
  {
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << key;
    elle::printf("[representation %s] %s\n", N, archive);
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

  // DES.
  test_represent_n<1, infinit::cryptography::cipher::Algorithm::des, 123>();
  // 2 DES.
  test_represent_n<2, infinit::cryptography::cipher::Algorithm::des2, 31>();
  // Triple DES.
  test_represent_n<3, infinit::cryptography::cipher::Algorithm::des3, 191>();
  // DES X.
  test_represent_n<4, infinit::cryptography::cipher::Algorithm::desx, 4090>();
  // IDEA.
  test_represent_n<5, infinit::cryptography::cipher::Algorithm::idea, 2056>();
  // RC2.
  test_represent_n<6, infinit::cryptography::cipher::Algorithm::rc2, 29>();
  // Blowfish.
  test_represent_n<7, infinit::cryptography::cipher::Algorithm::blowfish, 69>();
  // CAST5.
  test_represent_n<8, infinit::cryptography::cipher::Algorithm::cast5, 4720>();
  // AES128.
  test_represent_n<9, infinit::cryptography::cipher::Algorithm::aes128, 830>();
  // AES192.
  test_represent_n<10, infinit::cryptography::cipher::Algorithm::aes192, 921>();
  // AES256.
  test_represent_n<11, infinit::cryptography::cipher::Algorithm::aes256, 144>();
}

/*---------.
| Generate |
`---------*/

template <infinit::cryptography::cipher::Algorithm A,
          elle::Natural32 L>
infinit::cryptography::SecretKey
test_generate_x()
{
  infinit::cryptography::SecretKey key =
    infinit::cryptography::SecretKey::generate(A, L);

  return (key);
}

static
void
test_generate()
{
  // DES.
  test_generate_x<infinit::cryptography::cipher::Algorithm::des, 251>();
  // 2-DES.
  test_generate_x<infinit::cryptography::cipher::Algorithm::des2, 120>();
  // Triple DES.
  test_generate_x<infinit::cryptography::cipher::Algorithm::des3, 12>();
  // DES X.
  test_generate_x<infinit::cryptography::cipher::Algorithm::desx, 1281>();
  // IDEA.
  test_generate_x<infinit::cryptography::cipher::Algorithm::idea, 319>();
  // RC2.
  test_generate_x<infinit::cryptography::cipher::Algorithm::rc2, 3019>();
  // Blowfish.
  test_generate_x<infinit::cryptography::cipher::Algorithm::blowfish, 3019>();
  // CAST5.
  test_generate_x<infinit::cryptography::cipher::Algorithm::cast5, 417>();
  // AES128.
  test_generate_x<infinit::cryptography::cipher::Algorithm::aes128, 918>();
  // AES192.
  test_generate_x<infinit::cryptography::cipher::Algorithm::aes192, 124>();
  // AES256.
  test_generate_x<infinit::cryptography::cipher::Algorithm::aes256, 1083>();
}

/*----------.
| Construct |
`----------*/

static
infinit::cryptography::SecretKey
test_construct_aes128()
{
  infinit::cryptography::SecretKey key1(
    infinit::cryptography::cipher::Algorithm::aes128,
    "chiche",
    infinit::cryptography::oneway::Algorithm::md5);

  infinit::cryptography::SecretKey key2(
    infinit::cryptography::cipher::Algorithm::aes128,
    "chiche",
    infinit::cryptography::oneway::Algorithm::sha256);

  BOOST_CHECK_NE(key1, key2);

  infinit::cryptography::SecretKey key3(
    infinit::cryptography::cipher::Algorithm::aes128,
    "chiche",
    infinit::cryptography::oneway::Algorithm::sha256);

  BOOST_CHECK_EQUAL(key2, key3);

  infinit::cryptography::SecretKey key4 =
    test_generate_x<infinit::cryptography::cipher::Algorithm::aes128, 128>();

  return (key4);
}

static
void
test_construct()
{
  // AES128.
  infinit::cryptography::SecretKey key1 = test_construct_aes128();

  // SecretKey copy.
  infinit::cryptography::SecretKey key2(key1);

  BOOST_CHECK_EQUAL(key1, key2);

  // SecretKey move.
  infinit::cryptography::SecretKey key3(std::move(key1));

  BOOST_CHECK_EQUAL(key2, key3);
}

/*--------.
| Operate |
`--------*/

static
void
test_operate_idea()
{
  infinit::cryptography::SecretKey key =
    test_generate_x<infinit::cryptography::cipher::Algorithm::idea, 512>();

  // Encrypt/decrypt with plain.
  {
    elle::String const input = "Chier du foutre!";
    infinit::cryptography::Code code =
      key.encrypt(
        infinit::cryptography::Plain{input});
    infinit::cryptography::Clear clear = key.decrypt(code);
    elle::String const output(clear.buffer().string());

    BOOST_CHECK_EQUAL(input, output);
  }

  // Encrypt/decrypt with complex type.
  {
    Sample const input(
      42, infinit::cryptography::random::generate<elle::String>(32893));
    infinit::cryptography::Code code = key.encrypt(input);
    Sample const output = key.decrypt<Sample>(code);

    BOOST_CHECK_EQUAL(input, output);
  }
}

static
void
test_operate()
{
  // IDEA.
  test_operate_idea();
}

/*----------.
| Serialize |
`----------*/

template <infinit::cryptography::cipher::Algorithm A,
          elle::Natural32 L>
void
test_serialize_x(elle::String const& R)
{
  // Serialize/deserialize.
  {
    infinit::cryptography::SecretKey key1 = test_generate_x<A, L>();

    elle::String archive;
    elle::serialize::to_string(archive) << key1;

    auto extractor = elle::serialize::from_string(archive);
    infinit::cryptography::SecretKey key2(extractor);

    BOOST_CHECK_EQUAL(key1, key2);
  }

  // Deserialize from the hard-coded [representation R]: useful for detecting
  // changes in formats.
  {
    elle::String archive1 = R;
    auto extractor =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive1);
    infinit::cryptography::SecretKey key(extractor);

    elle::String archive2;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive2) << key;

    BOOST_CHECK_EQUAL(archive1, archive2);
  }
}

static
void
test_serialize()
{
  // DES based on [representation 1].
  test_serialize_x<infinit::cryptography::cipher::Algorithm::des, 491>("AAAAAAAADwAAAAAAAAAaPkHuqG5X3Y8nlvJ2vP4EAA==");
  // 2-DES based on [representation 2].
  test_serialize_x<infinit::cryptography::cipher::Algorithm::des2, 82>("AAABAAAAAwAAAAAAAACtSXgEAA==");
  // Triple DES based on [representation 3].
  test_serialize_x<infinit::cryptography::cipher::Algorithm::des3, 1074>("AAACAAAAFwAAAAAAAADj47h5QvqrtSt5tv1Pc7OE8INIaVJH7AQA");
  // DES X based on [representation 4].
  test_serialize_x<infinit::cryptography::cipher::Algorithm::desx, 193>("AAADAAAA/wEAAAAAAACKo7CcohFWQOXJgZgjzKYWULWovcX4UwmfFKNBo/s+BfOg8zhVXwmpSrvyUP8SKa7vdl4pMslP53sJmy3QqNl/QulgeNwcLXYJIcuAQZyUk2igh/2DI3d6mxP1BlaU6yluH66Zza+lzDmy986reL4AdVo54d5W8tRgbUqzyAiz2RlQFfyuW3iQEXDxqjK5mLeVkv5PGVSVDlbbWNG9oDHtfGLfequyoz/JYuXZxZNl55EbyKCezRJ0zuv2eskolRwALmbLV2crmW548FOrAoCAgxw4M+9SQzXq8ZSZiL8ARoDP3tlVsuy8+yWcXI+VR0y0ihbnxdTT+FvInk9wZoJ81xm8fMmYpEdayXCb3p+yBl6xZYTaIyhzSyVX+BDrBgpu5U5lRS3drNXcE4LOYf11IOJTEtAPjAiMfN6uSFpdljFUjBznvKb1DVGjObJqUlPb3h5BwYADIwnY+HbgqwKfLmnOSwzp06WAeC8y1zanzb7/yHclXQqeDoy4oTQ6QdZ5GXsgnmS0wnhsVVmpfvQdOgHRXpjLwVLW8oRkDpxvPSyIBzkgEIgd9A/vB6D1sC0HU/cTv5Kl5U59tW9X0yFulVd0MMeLcEcLSsZ1Us5bgb0jZyYCjQaAd1L5Twn1wVpmJfbBCC2b76AmO4svoSlQGfCSwA0PSc48X2JqWCjwBAA=");
  // IDEA based on [representation 5].
  test_serialize_x<infinit::cryptography::cipher::Algorithm::idea, 4091>("AAAEAAAAAQEAAAAAAADbEcfxyz7gmMhd4DaPeA5J/mflfdqhrRDno0Z47ndrodiBq95cJCr4GaS9LxmGq6A8Wfb6AJB2oc8YAiPa9ZSvMZK3vQ8XlYSMBFL3172XigIFW1VfAql9MGAmnYM1Ush9W5UKfdwWpo1BwYCxTJU9SOB3ytP7H8dnBTs5UevvGwh2sFY3A3rv6MOv5M0J9dyrdJ7O86QZzPlfwUdJsEBXc86ZlUjYpwvGnTZeMapT6ArsUovFNkkzBEUc95I4z2GT9tUlKq2QGiv8gExKCHam8hOJMRDbppvj+KXPSWLz9q13i1P8hUWKS/0lkiEksGI1JYjhkTN7UwxhfQkGRohaGQQA");
  // RC2 based on [representation 6].
  test_serialize_x<infinit::cryptography::cipher::Algorithm::rc2, 12>("AAAFAAAAAwAAAAAAAADgQysEAA==");
  // Blowfish based on [representation 7].
  test_serialize_x<infinit::cryptography::cipher::Algorithm::blowfish, 223>("AAAGAAAACAAAAAAAAAAUeiXrxAx9uQQA");
  // CAST5 based on [representation 8].
  test_serialize_x<infinit::cryptography::cipher::Algorithm::cast5, 1398>("AAAHAAAATgIAAAAAAAC08hK1AsxQluKL93G+RjB+XgRS16kFZtn37mVxAbrJOwPbVxeQTS5nysGqv4gcuFFQTJTnrVJ049ctC1txpO9Maj/KCnoiFGbn4Lz7i6xeHZ873WbeeIy3RoRh+YynKFtas8Y18Z5tdNYHJ/wKPpWKiZXStOjHzk7zGb2S/OPEg5KaFJT3Pa7Zu34yMfFLZfu9tiUBCiyKSYg3GVP5f3lcc3szHXIk5dUxxoKYC7wAWlyQ/vjwtCNCSJ8Sqt/AbDAjaFikYDLOMquinxq8Wkgb/7y5jgz+Diof6+VTdhG1+6xgUdJJXJ2Oe3hKCW0Z/qIzSxw1xXJhxJl4FQVso4UiQKTmM6y/ALRfSOIVpu4tT0Cpw3V+kKN0PDHLl5i93W/W5v3LjbJZdV7dwNlwCaXbkUpKTJcf4oilM9T5aOfZ78Bi7+SY8zjWDX+ylbPgx9tYsJihGgzcamG4rO7UsmzMrJq5yg/DnJcJ8QFzw9MLOlgHcyH3CX9TI5wGxOyejP3K3D/wwVZC+Mhup9aa5kkmARcLPwQEpUEJsXNrIccAhXaRfXgA2aAbpGdVb3DaJVOAd9zsO20D1dn9gmiDjzJifwwcyW4dBRzXuDD5eRhl4ivm1KWMKv3vt5nr0SgSxACEJI8NjZLEnZPc7++QFpPnqr1Ys3sAz/uUszrb58y9XsZvO4aZakWZhqGBIh4pOvq09nyuu1BDzFFz0zaLZuvR6qCOCFEg8LC8CsqgwfhToy/Xfz+760scAhEyhf4U836hmiIXNhcszIvbtH3ElwQA");
  // AES128 based on [representation 9].
  test_serialize_x<infinit::cryptography::cipher::Algorithm::aes128, 94>("AAAIAAAAZwAAAAAAAADUoRvMwLoz7hV00rAlsTIHW9zEMnZwihNGMLomB0a4J4AxO4nXtk1mlWVehhjcjHW6E1RzsTVKNsa9mcGy/i0NoyIh6ZbmCVpgXuh+ognaO4U2lam2ViOicPAsp7hIOIKGnXYZpwxPBAA=");
  // AES192 based on [representation 10].
  test_serialize_x<infinit::cryptography::cipher::Algorithm::aes192, 320>("AAAJAAAAcwAAAAAAAABHweCXb7/EjZXX7TWZJxZSxy91PdB/qziAweoTJ+TEVAOwMZdGms4anNwYUkr2eHvg0gnksWDrEmjq7HZa+qbtt8NDcaoAtvJAowbf7doVV+zOQg7LmIDPdXu5HJ6Fd/MdsCCplXN55WBmwZ2UiNZS5MCGBAA=");
  // AES256 based on [representation 11].
  test_serialize_x<infinit::cryptography::cipher::Algorithm::aes256, 2338>("AAAKAAAAEgAAAAAAAADxKyeb9Ypn4DNuuMihYjzNvDQEAA==");
}

/*-----.
| Main |
`-----*/

ELLE_TEST_SUITE()
{
  boost::unit_test::test_suite* suite = BOOST_TEST_SUITE("SecretKey");

  suite->add(BOOST_TEST_CASE(test_represent));
  suite->add(BOOST_TEST_CASE(test_generate));
  suite->add(BOOST_TEST_CASE(test_construct));
  suite->add(BOOST_TEST_CASE(test_operate));
  suite->add(BOOST_TEST_CASE(test_serialize));

  boost::unit_test::framework::master_test_suite().add(suite);
}
