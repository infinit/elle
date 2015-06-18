#include "cryptography.hh"
#include "Sample.hh"

#include <cryptography/SecretKey.hh>
#include <cryptography/Exception.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/random.hh>

#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

/*----------.
| Represent |
`----------*/

static elle::String const _message("I don't know what we're yelling about!");

template <elle::Natural32 N,
          elle::Natural32 L,
          infinit::cryptography::Cipher C,
          infinit::cryptography::Mode M,
          infinit::cryptography::Oneway O>
void
test_represent_n()
{
  infinit::cryptography::SecretKey key =
    infinit::cryptography::secretkey::generate(L, C, M, O);

  // N.1)
  {
    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << key;
    elle::printf("[representation %s.1] %s\n", N, archive);
  }

  // N.2)
  {
    infinit::cryptography::Code code = key.encrypt(_message);

    elle::String archive;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive) << code;
    elle::printf("[representation %s.2] %s\n", N, archive);
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
  test_represent_n<1,
                   123,
                   infinit::cryptography::Cipher::des,
                   infinit::cryptography::Mode::ofb,
                   infinit::cryptography::Oneway::md5>();
  // 2 DES.
  test_represent_n<2,
                   31,
                   infinit::cryptography::Cipher::des2,
                   infinit::cryptography::Mode::ecb,
                   infinit::cryptography::Oneway::sha1>();
  // Triple DES.
  test_represent_n<3,
                   191,
                   infinit::cryptography::Cipher::des3,
                   infinit::cryptography::Mode::cfb,
                   infinit::cryptography::Oneway::sha384>();
  // DES X.
  test_represent_n<4,
                   4090,
                   infinit::cryptography::Cipher::desx,
                   infinit::cryptography::Mode::cbc,
                   infinit::cryptography::Oneway::sha>();
  // IDEA.
  test_represent_n<5,
                   2056,
                   infinit::cryptography::Cipher::idea,
                   infinit::cryptography::Mode::ecb,
                   infinit::cryptography::Oneway::sha224>();
  // RC2.
  test_represent_n<6,
                   29,
                   infinit::cryptography::Cipher::rc2,
                   infinit::cryptography::Mode::ofb,
                   infinit::cryptography::Oneway::sha512>();
  // Blowfish.
  test_represent_n<7,
                   69,
                   infinit::cryptography::Cipher::blowfish,
                   infinit::cryptography::Mode::cbc,
                   infinit::cryptography::Oneway::sha1>();
  // CAST5.
  test_represent_n<8,
                   4720,
                   infinit::cryptography::Cipher::cast5,
                   infinit::cryptography::Mode::cfb,
                   infinit::cryptography::Oneway::sha>();
  // AES128.
  test_represent_n<9,
                   830,
                   infinit::cryptography::Cipher::aes128,
                   infinit::cryptography::Mode::cbc,
                   infinit::cryptography::Oneway::md5>();
  // AES192.
  test_represent_n<10,
                   921,
                   infinit::cryptography::Cipher::aes192,
                   infinit::cryptography::Mode::ofb,
                   infinit::cryptography::Oneway::sha224>();
  // AES256.
  test_represent_n<11,
                   144,
                   infinit::cryptography::Cipher::aes256,
                   infinit::cryptography::Mode::ecb,
                   infinit::cryptography::Oneway::sha384>();
}

/*---------.
| Generate |
`---------*/

template <elle::Natural32 L,
          infinit::cryptography::Cipher C,
          infinit::cryptography::Mode M,
          infinit::cryptography::Oneway O>
infinit::cryptography::SecretKey
test_generate_x()
{
  infinit::cryptography::SecretKey key =
    infinit::cryptography::secretkey::generate(L, C, M, O);

  return (key);
}

static
void
test_generate()
{
  // DES.
  test_generate_x<251,
                  infinit::cryptography::Cipher::des,
                  infinit::cryptography::Mode::cbc,
                  infinit::cryptography::Oneway::sha>();
  // 2 DES.
  test_generate_x<120,
                  infinit::cryptography::Cipher::des2,
                  infinit::cryptography::Mode::ofb,
                  infinit::cryptography::Oneway::sha224>();
  // Triple DES.
  test_generate_x<12,
                  infinit::cryptography::Cipher::des3,
                  infinit::cryptography::Mode::ecb,
                  infinit::cryptography::Oneway::md5>();
  // DES X.
  test_generate_x<1281,
                  infinit::cryptography::Cipher::desx,
                  infinit::cryptography::Mode::cfb,
                  infinit::cryptography::Oneway::sha1>();
  // IDEA.
  test_generate_x<319,
                  infinit::cryptography::Cipher::idea,
                  infinit::cryptography::Mode::ofb,
                  infinit::cryptography::Oneway::sha256>();
  // RC2.
  test_generate_x<3016,
                  infinit::cryptography::Cipher::rc2,
                  infinit::cryptography::Mode::cbc,
                  infinit::cryptography::Oneway::sha>();
  // Blowfish.
  test_generate_x<812,
                  infinit::cryptography::Cipher::blowfish,
                  infinit::cryptography::Mode::ecb,
                  infinit::cryptography::Oneway::md5>();
  // CAST5.
  test_generate_x<317,
                  infinit::cryptography::Cipher::cast5,
                  infinit::cryptography::Mode::ofb,
                  infinit::cryptography::Oneway::sha224>();
  // AES128.
  test_generate_x<918,
                  infinit::cryptography::Cipher::aes128,
                  infinit::cryptography::Mode::cfb,
                  infinit::cryptography::Oneway::sha1>();
  // AES192.
  test_generate_x<124,
                  infinit::cryptography::Cipher::aes192,
                  infinit::cryptography::Mode::ecb,
                  infinit::cryptography::Oneway::sha384>();
  // AES256.
  test_generate_x<1083,
                  infinit::cryptography::Cipher::aes256,
                  infinit::cryptography::Mode::cfb,
                  infinit::cryptography::Oneway::sha1>();
}

/*----------.
| Construct |
`----------*/

static
infinit::cryptography::SecretKey
_test_construct_aes128()
{
  infinit::cryptography::SecretKey key1(
    "chiche",
    infinit::cryptography::Cipher::aes128,
    infinit::cryptography::Mode::ofb,
    infinit::cryptography::Oneway::md5);

  infinit::cryptography::SecretKey key2(
    "chiche",
    infinit::cryptography::Cipher::aes128,
    infinit::cryptography::Mode::ecb,
    infinit::cryptography::Oneway::sha256);

  BOOST_CHECK_NE(key1, key2);

  infinit::cryptography::SecretKey key3(
    "chiche",
    infinit::cryptography::Cipher::aes128,
    infinit::cryptography::Mode::ecb,
    infinit::cryptography::Oneway::sha256);

  BOOST_CHECK_EQUAL(key2, key3);

  infinit::cryptography::SecretKey key4 =
    test_generate_x<128,
                    infinit::cryptography::Cipher::aes128,
                    infinit::cryptography::Mode::cbc,
                    infinit::cryptography::Oneway::sha256>();

  return (key4);
}

static
void
test_construct()
{
  // AES128.
  infinit::cryptography::SecretKey key1 = _test_construct_aes128();

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
_test_operate_idea()
{
  infinit::cryptography::SecretKey key =
    test_generate_x<512,
                    infinit::cryptography::Cipher::idea,
                    infinit::cryptography::Mode::cbc,
                    infinit::cryptography::Oneway::sha256>();

  // Encrypt/decrypt with plain.
  {
    elle::String const input = "Chie du foutre!";
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
  _test_operate_idea();
}

/*----------.
| Serialize |
`----------*/

// Legacy testing function for the SecretKey class in its format 0.
template <infinit::cryptography::Cipher C,
          elle::Natural32 L>
void
test_serialize_x_0(elle::String const& R)
{
  // Serialize/deserialize.
  {
    infinit::cryptography::SecretKey key1 =
      test_generate_x<L,
                      C,
                      infinit::cryptography::Mode::cbc,
                      infinit::cryptography::Oneway::sha256>();

    elle::String archive;
    elle::serialize::to_string(archive) << key1;

    auto extractor = elle::serialize::from_string(archive);
    infinit::cryptography::SecretKey key2(extractor);

    BOOST_CHECK_EQUAL(key1, key2);
  }

  // Deserialize from the hard-coded R representation: useful for detecting
  // changes in formats.
  {
    elle::String archive1 = R;

    auto extractor1 =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive1);
    infinit::cryptography::SecretKey key1(extractor1);

    infinit::cryptography::Code code = key1.encrypt(_message);
    elle::String clear = key1.decrypt<elle::String>(code);

    BOOST_CHECK_EQUAL(_message, clear);

    elle::String archive2;
    elle::serialize::to_string<
      elle::serialize::OutputBase64Archive>(archive2) << key1;

    auto extractor2 =
      elle::serialize::from_string<
        elle::serialize::InputBase64Archive>(archive2);
    infinit::cryptography::SecretKey key2(extractor2);

    BOOST_CHECK_EQUAL(key1, key2);
  }
}

// Testing function for the newer formats of SecretKey.
static
void
test_serialize_x_n(elle::String const& R1,
                   elle::String const& R2)
{
  // Deserialize the key from R1.
  auto extractor1 =
    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(R1);
  infinit::cryptography::SecretKey key(extractor1);

  // Deserialize the code from R2.
  auto extractor2 =
    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(R2);
  infinit::cryptography::Code code(extractor2);

  // Decrypt the code.
  elle::String message = key.decrypt<elle::String>(code);

  BOOST_CHECK_EQUAL(_message, message);
}

static
void
test_serialize()
{
  // Old format: 0
  {
    // DES.
    test_serialize_x_0<infinit::cryptography::Cipher::des, 491>("AAAAAAAADwAAAAAAAAAaPkHuqG5X3Y8nlvJ2vP4EAA==");
    // 2-DES.
    test_serialize_x_0<infinit::cryptography::Cipher::des2, 82>("AAABAAAAAwAAAAAAAACtSXgEAA==");
    // Triple DES.
    test_serialize_x_0<infinit::cryptography::Cipher::des3, 1074>("AAACAAAAFwAAAAAAAADj47h5QvqrtSt5tv1Pc7OE8INIaVJH7AQA");
    // DES X.
    test_serialize_x_0<infinit::cryptography::Cipher::desx, 193>("AAADAAAA/wEAAAAAAACKo7CcohFWQOXJgZgjzKYWULWovcX4UwmfFKNBo/s+BfOg8zhVXwmpSrvyUP8SKa7vdl4pMslP53sJmy3QqNl/QulgeNwcLXYJIcuAQZyUk2igh/2DI3d6mxP1BlaU6yluH66Zza+lzDmy986reL4AdVo54d5W8tRgbUqzyAiz2RlQFfyuW3iQEXDxqjK5mLeVkv5PGVSVDlbbWNG9oDHtfGLfequyoz/JYuXZxZNl55EbyKCezRJ0zuv2eskolRwALmbLV2crmW548FOrAoCAgxw4M+9SQzXq8ZSZiL8ARoDP3tlVsuy8+yWcXI+VR0y0ihbnxdTT+FvInk9wZoJ81xm8fMmYpEdayXCb3p+yBl6xZYTaIyhzSyVX+BDrBgpu5U5lRS3drNXcE4LOYf11IOJTEtAPjAiMfN6uSFpdljFUjBznvKb1DVGjObJqUlPb3h5BwYADIwnY+HbgqwKfLmnOSwzp06WAeC8y1zanzb7/yHclXQqeDoy4oTQ6QdZ5GXsgnmS0wnhsVVmpfvQdOgHRXpjLwVLW8oRkDpxvPSyIBzkgEIgd9A/vB6D1sC0HU/cTv5Kl5U59tW9X0yFulVd0MMeLcEcLSsZ1Us5bgb0jZyYCjQaAd1L5Twn1wVpmJfbBCC2b76AmO4svoSlQGfCSwA0PSc48X2JqWCjwBAA=");
    // IDEA.
    test_serialize_x_0<infinit::cryptography::Cipher::idea, 4091>("AAAEAAAAAQEAAAAAAADbEcfxyz7gmMhd4DaPeA5J/mflfdqhrRDno0Z47ndrodiBq95cJCr4GaS9LxmGq6A8Wfb6AJB2oc8YAiPa9ZSvMZK3vQ8XlYSMBFL3172XigIFW1VfAql9MGAmnYM1Ush9W5UKfdwWpo1BwYCxTJU9SOB3ytP7H8dnBTs5UevvGwh2sFY3A3rv6MOv5M0J9dyrdJ7O86QZzPlfwUdJsEBXc86ZlUjYpwvGnTZeMapT6ArsUovFNkkzBEUc95I4z2GT9tUlKq2QGiv8gExKCHam8hOJMRDbppvj+KXPSWLz9q13i1P8hUWKS/0lkiEksGI1JYjhkTN7UwxhfQkGRohaGQQA");
    // RC2.
    test_serialize_x_0<infinit::cryptography::Cipher::rc2, 12>("AAAFAAAAAwAAAAAAAADgQysEAA==");
    // Blowfish.
    test_serialize_x_0<infinit::cryptography::Cipher::blowfish, 223>("AAAGAAAACAAAAAAAAAAUeiXrxAx9uQQA");
    // CAST5.
    test_serialize_x_0<infinit::cryptography::Cipher::cast5, 1398>("AAAHAAAATgIAAAAAAAC08hK1AsxQluKL93G+RjB+XgRS16kFZtn37mVxAbrJOwPbVxeQTS5nysGqv4gcuFFQTJTnrVJ049ctC1txpO9Maj/KCnoiFGbn4Lz7i6xeHZ873WbeeIy3RoRh+YynKFtas8Y18Z5tdNYHJ/wKPpWKiZXStOjHzk7zGb2S/OPEg5KaFJT3Pa7Zu34yMfFLZfu9tiUBCiyKSYg3GVP5f3lcc3szHXIk5dUxxoKYC7wAWlyQ/vjwtCNCSJ8Sqt/AbDAjaFikYDLOMquinxq8Wkgb/7y5jgz+Diof6+VTdhG1+6xgUdJJXJ2Oe3hKCW0Z/qIzSxw1xXJhxJl4FQVso4UiQKTmM6y/ALRfSOIVpu4tT0Cpw3V+kKN0PDHLl5i93W/W5v3LjbJZdV7dwNlwCaXbkUpKTJcf4oilM9T5aOfZ78Bi7+SY8zjWDX+ylbPgx9tYsJihGgzcamG4rO7UsmzMrJq5yg/DnJcJ8QFzw9MLOlgHcyH3CX9TI5wGxOyejP3K3D/wwVZC+Mhup9aa5kkmARcLPwQEpUEJsXNrIccAhXaRfXgA2aAbpGdVb3DaJVOAd9zsO20D1dn9gmiDjzJifwwcyW4dBRzXuDD5eRhl4ivm1KWMKv3vt5nr0SgSxACEJI8NjZLEnZPc7++QFpPnqr1Ys3sAz/uUszrb58y9XsZvO4aZakWZhqGBIh4pOvq09nyuu1BDzFFz0zaLZuvR6qCOCFEg8LC8CsqgwfhToy/Xfz+760scAhEyhf4U836hmiIXNhcszIvbtH3ElwQA");
    // AES128.
    test_serialize_x_0<infinit::cryptography::Cipher::aes128, 94>("AAAIAAAAZwAAAAAAAADUoRvMwLoz7hV00rAlsTIHW9zEMnZwihNGMLomB0a4J4AxO4nXtk1mlWVehhjcjHW\
6E1RzsTVKNsa9mcGy/i0NoyIh6ZbmCVpgXuh+ognaO4U2lam2ViOicPAsp7hIOIKGnXYZpwxPBAA=");
    // AES192.
    test_serialize_x_0<infinit::cryptography::Cipher::aes192, 320>("AAAJAAAAcwAAAAAAAABHweCXb7/EjZXX7TWZJxZSxy91PdB/qziAweoTJ+TEVAOwMZdGms4anNwYUkr2eH\
vg0gnksWDrEmjq7HZa+qbtt8NDcaoAtvJAowbf7doVV+zOQg7LmIDPdXu5HJ6Fd/MdsCCplXN55WBmwZ2UiNZS5MCGBAA=");
    // AES256.
    test_serialize_x_0<infinit::cryptography::Cipher::aes256, 2338>("AAAKAAAAEgAAAAAAAADxKyeb9Ypn4DNuuMihYjzNvDQEAA==");
  }

  // Newer formats: >= 1
  {
    // DES based on [representation 1].
    test_serialize_x_n("AQAAAA8AAAAAAAAAjMVaoAHoNFWNZ9VnHhJKAQAEAAAA",
                       "AAAAADoAAAAAAAAAU2FsdGVkX18xfk15z14820K4tfRK+eeO1kM55c5Yx36hE405BqR+JGFj6FdkylyTwvt07CItLOwWyQ==");
    // 2-DES based on [representation 2].
    test_serialize_x_n("AQAAAAMAAAAAAAAAAkkMAgACAAIA",
                       "AAAAAEAAAAAAAAAAU2FsdGVkX1/RYG5MihXMO5ealwQ7g0bAG1jLFEDHjEzhXWOrad7HQXxL5VvXafhURayZ5YjDYhwGKafUrWD71g==");
    // Triple DES based on [representation 3].
    test_serialize_x_n("AQAAABcAAAAAAAAAscHzIgUJPHkmhrs1NgEt8rltPpgS32oDAAMABQA=",
                       "AAAAADoAAAAAAAAAU2FsdGVkX1/gl8Gl2Lo1OcN1H3I/jKZhgQGUJw2jatOChks8rzP4TW28MX1KekpgODwCtfD+J5iYEw==");
    // DES X based on [representation 4].
    test_serialize_x_n("AQAAAP8BAAAAAAAACYbt6vJUYIXI9rQEctOYpr419U1ofXc4aEBELa19CXaPlKeZgEFQoOXJAOf93IjG2BcZ5dDVX0p1tne0UB1l0bW2kfOSMdPOUDe9H00qnpw8bqMlSHYXaByRvkB5UvHs+yB+uM/NiqJ8FcnC+R0Bg3ov2ad9RZLN+SQ8IbRu77RSM7z79XOWIur01iRr3Z1Ee8sSHrmbKn/S9ojM8CXX9Skf7nUwawrikICTbaiJBYLCOSDyE7v1agvPJgaOmhk6VfNzewRvbgBd9u1VzDGR84bAUTTXB3YFOMnaXLMOMPS+ZgWakVPL3F8KILBvw2Ecvb14yAVHx/v38D/COnP2XxrICijBA7hx7K4tf4ln8CLjXhzKuiJJzZTVDz5VJa+mkfGQ+Vc2Cjfjn+ySrWfjoGqnTwzusTFoTqqvpd0VtMBJzGDbzgHKio1U1JNChkAHha11LEaXmKTdwi1y2ThSNNprRBoDwsZZo8C55aFtrhx9qsEcnM9kVVPI5pg5xRs/7XFn5lc/zgm7AQF/sQXA9dDuamNf/8GM68P7GpBWtuGdxSlOsAhBzPpe6PemIUCLXLI6QUYMSQ+6gw6Q84dW1sx6TBgooyNFecPYetiaPpmFUvjDaRDD035BiPj9HyrXnq07qKiSDekUnMlS7tgMPH1Z/P5LS+HhHwLxYpBoLQQAAQABAA==",
                       "AAAAAEAAAAAAAAAAU2FsdGVkX1/FlXIoYKWY2C5ucJmFbRqdxYiwminM3TmoSbd1jnO+YJAAXDz/ddgtDLNWQNHTGhxzziqvxc6U5w==");
    // IDEA based on [representation 5].
    test_serialize_x_n("AQAAAAEBAAAAAAAAQLVvIab7pao//lwKjrNmEN5MDBI5Mi6ermp3OSUtCchGfVgykaA8f3RWA+igCm7k+M/NNMqq89RRJJmiCjun5L7mk7+T4xamnznfeNP80C35avSAj/t2hqAz9ywd3Hmj8vtnuX98CfW6EIDGauws1/ybGjXyPCpC+djt3oiNeCV16hoVK5TgMJkRq8nEO/VU3ZULL5ppA7tRWObSGofJ3NKV8eS7FEHAAgXA6/9V2UsyynYJn+/cGlcTYP2zcYdbtlSzteVs8RGBxFqRgV7DxCUTCF54Yigtix2tin0lBULRJZxKu7hkmZfmkLFTIv0hBRLGrkvk12OH5GA5F89U1TAFAAIAAwA=",
                       "AAAAAEAAAAAAAAAAU2FsdGVkX19fnDB+md/7HFIq1qat9fyXuMtrHv/i0aKfn1Sjd9sksRZMD1MQGZLsaUROoN8Y2m44X+h0xgExVw==");
    // RC2 based on [representation 6].
    test_serialize_x_n("AQAAAAMAAAAAAAAAPX+rBgAEAAYA",
                       "AAAAADoAAAAAAAAAU2FsdGVkX1+5nB653wIJSBnI9hDqAcyU7uymuJFxsOTC9R2wHVsgko7PqkMcZ9u7fHTwTmxi+KDueg==");
    // Blowfish based on [representation 7].
    test_serialize_x_n("AQAAAAgAAAAAAAAAjiGXG+PY5iYHAAEAAgA=",
                       "AAAAAEAAAAAAAAAAU2FsdGVkX1/GeZaFojF/fGNk6S10UizTANiozkJNOwpdH6TWTReXKkuuA615DvieJ29i/2eKZ2DyojLEmEnEzQ==");
    // CAST5 based on [representation 8].
    test_serialize_x_n("AQAAAE4CAAAAAAAAKV3P70YypjdjycGDUUG/LnK8LzMuwt1K4WXPIQ+Vg9JDSCBx8YvErBk+IlkZKYmchCERMcajIO63Do1pvZvUEVjLIwxtWVTs/i2UZ3Iv4OY9zyiqnggZRieXoVoy2Tj9fp7U0lPE+sGkm1wmVUNEZnDBGNPaaegZoEDA5JP5KYaGybPtiwrPS/cahXKvfMudgPEwBZ0ucf91VbuF7GFfFn2tLaKwSlji0vdTmA9dQECnq5zbufap7S7MUZoMqjrurd++xSoPq4vmCCYnks3z2Wztde7DBY0V7I5LQaZjFWJCnHAHf+jAotccXiqvnUMkMRhsLSl7qrTOiCPO2deQ/bqPsw+nIDrL28ha7JC/mwi+3QlCWakReuTDQBJmIptumko2XDmqnrQDeM42Qewa481KKGrnfeR+xzmcZYsn/Xe/FwPTHCfydoQwC6eGhUebhFrA4xPVpPlXSFNlSd9TzYW0tE2VxVEbDRKXBHjtnXztw/5uz3zTzQywkE1tZj3CoOVJI15RQy/FFhoEbxtamYtupvNb2wfNnlFWkia7dcNs/8MQBZ0eJVCl6LlQHnUSE6QZ24NPjcQ0O06IT/Z/mew9qtkXfQXCEyPdTAWewoHQcGWYM1oX2mJ3oDbJX6P6L31PKtTIo25CdqX/mKUC58SEsthL3vfMFugEXYTPUc3xfrGQf09LdwDuYp5B3sOfYpjKSgha9hDvq6gs2U0MUtAhCku0GLupHiGXm2XE4sYfINlVdc3M6kWQcpInjW47GALVKLl8NDsMl5cC0aUIAAMAAQA=",
                       "AAAAADoAAAAAAAAAU2FsdGVkX1/Yz02jGE/raHWUu6TBFGlSoS7yHo409ZIC8Clr+GZM2b9XOf8LSUjJQdW4T27ZnR52Vg==");
    // AES128 based on [representation 9].
    test_serialize_x_n("AQAAAGcAAAAAAAAALYIclorj5wkAjZ4yyHxZ7fxlTTkDK0ViYzVkRrsFa1JwfWW5m9Of1OExlhtfZer6F0DLLlrmZ18j0E4mgexYr9X5coRHc5arS5Cis7eRv2pu0hV9TyY2LPBQ6irPNVyhS+c7yXz/oQkAAQAAAA==",
                       "AAAAAEAAAAAAAAAAU2FsdGVkX1+s85eUWu1Knoe0WohZLRNMyY1F7F5K+yPNlU6DNHIK40E3GWdfaXfaPX9qSCSnQQ+oxrv24brr+w==");
    // AES192 based on [representation 10].
    test_serialize_x_n("AQAAAHMAAAAAAAAAdhEnTprkprmTv3RJ4WJ9nIuAq9xllP0bov4LsP0p3WWAlLF9c1hLzujWftQ0kMIqCt1SZTxmY+S4x82zKGA7o8CKbqm7zSZbxx+XGvRveQ44TmyB6JTTUen/ITefUa6Ssj4+Wot9hH754ig+Um6TOe6QRAoABAADAA==",
                       "AAAAADoAAAAAAAAAU2FsdGVkX19TGLi4RClRT2ddJ9oci9VfyEjCDJTK3ngBOMQ8SWF1n8W7UAl8BvRO6LtcFc3t1G2fKQ==");
    // AES256 based on [representation 11].
    test_serialize_x_n("AQAAABIAAAAAAAAAPQIIM2i04XmvufB0O0gX7hLrCwACAAUA",
                       "AAAAAEAAAAAAAAAAU2FsdGVkX18HwvtxMF/XpVERWO5zxf7taEBfqMprPBO5qyyOBIrY0vfD6c9pW8/2BK40U9jDitdT/d5Uz8GolA==");
  }
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
