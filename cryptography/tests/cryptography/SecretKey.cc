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
  _test_operate_idea();
}

/*----------.
| Serialize |
`----------*/

static
void
test_serialize_x(elle::String const& R1,
                 elle::String const& R2)
{
  // Deserialize the key from [representation R.1].
  auto extractor1 =
    elle::serialize::from_string<
      elle::serialize::InputBase64Archive>(R1);
  infinit::cryptography::SecretKey key(extractor1);

  // Deserialize the code from [representation R.2].
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
  // DES based on [representation 1].
  test_serialize_x("AAAAAA8AAAAAAAAAhjxWOwxDpDWVKyDzDnSXAQAEAAAA",
                   "AAAAADoAAAAAAAAAU2FsdGVkX18HX/dWoDGDYDpuQqSY+LHyVrq7DBIJRH0EuDQyCOGdWyM0bD8kiKBIA7W3PhuUascXEQ==");
  // 2-DES based on [representation 2].
  test_serialize_x("AAAAAAMAAAAAAAAAI+b/AgACAAIA",
                   "AAAAAEAAAAAAAAAAU2FsdGVkX190LWXKvrQGMni/AB9WPhGbwXGrbOQxfrZ8rXQFKgio0qqEjVQKIfOdS/KSTyiTufeEMUuheD7YHA==");
  // Triple DES based on [representation 3].
  test_serialize_x("AAAAABcAAAAAAAAAeX0h9z2RKj7GHgJ1eaIyIt4C83W8V98DAAMABQA=",
                   "AAAAADoAAAAAAAAAU2FsdGVkX1/4r+Myw6DHvO3aoq4etEQw2UpRJkn6m1fVJkaoPbsb2asR3BLnd+Oy7Uyq/G+RJ5Rm7Q==");
  // DES X based on [representation 4].
  test_serialize_x("AAAAAP8BAAAAAAAA8fBsCsToxlR3l2wM3BBzAPG7aHGOoxn2mnjraVhEjC21Eo6hbGRHf9aTWTb1bgdlHMkQ+dPevnqbHXGTEGpoYaT1ZuBnk5McDUoNIA0+KRTQsnkj9KuM1GibT8cT0jnqCSAp07cjfulXLjqKat27lqsojHhAo2xhQSCUIl3pTymnLMHWQjVQ9jrG+WbTEunKxOoIF8VrOmXxiTThqqyrrEn4ite3RUAGVc/0MTOfcwkGN4HoQor+3j+8AyBod8Ks58LVgmBhfYdj7hnnMLNSXbiUwQEBStGW3JIevPE1lz5HvMXkpA8xFu69HAf85CcDWBQ7It+LtLP5vhqgMm3yhfM1DWwOqARlcYk1AKqkZdJO/JPpH2hdMQhev+qdxk9f/tivX5McyWK5aoIWpJpw+RM8w6jT2HcyxRXlEiD5KZW93g77wxuJhtmKl5cqNB9NTs3pBzGD2GEVGInHc5G4Sey6jesxTJgOcEOfO+EdvtpQ1gM5usD0sa5dfyy89quZQsNH6W9ZcHSvCUfaALdkOPdPZyV4c7aLQNahj1yv5UBEzuiVF1QEBG/8eE6QbU0ic2Wcr0+NDbrmb2VJyVMjvEd1/RYhUhgX87IieXw4WlZgxLk27dpmwa+nYJPgFuCcWeSh2tGoQ6G26lyHKiRXlnJadTiAbAsJKePuFGk+mgQAAQABAA==",
                   "AAAAAEAAAAAAAAAAU2FsdGVkX19zqhOGDZ007tb7twNbBHr/xQmji3xhHBkxqltzD8lkl8tVhRZyaXSBK+ve5JcSr40bjzlQ954G5g==");
  // IDEA based on [representation 5].
  test_serialize_x("AAAAAAEBAAAAAAAAYyS5HumWAwFBFTf+9wSvPuyz4YiaiHwMvsZDBpFF5UYb9f3lZiZBIsQ/mA/X6sB/Hd/m8EzBGh/qI97My7IXaAmQR//OktA8gfX9d3rbHmnupQ1WV2w+QDHD22pOmmkGrG/jCjZkCZkzQroYz30G6GSuZY0XQaSckJz1PqCPhIxWvTAVDV4AJVSBkgHn6ZuMnlOCtCq6XN/Dw+2uHIDzKFqGCmKHkWFoT8YSdZap3mkKu2zLLjl0y+mKFxtSo4PURFyOXb/e+zTECioN4XWyybDsE1W8aSh4eEuTiaY91cch7iGyspKlAV60C6XrMuUQXvTaSMKvk3rUpu47mX5JA9EFAAIAAwA=",
                   "AAAAAEAAAAAAAAAAU2FsdGVkX1+4eforYRhPaJ1a5+3fgmOQf2aB6qg4kwTKoV6Svef0SpGorZytj2Pq9be/BeUdOo4dLXNEV/aenA==");
  // RC2 based on [representation 6].
  test_serialize_x("AAAAAAMAAAAAAAAAKvDEBgAEAAYA",
                   "AAAAADoAAAAAAAAAU2FsdGVkX1+18DzzbcUKTCY78b68kbZ0VWX4u3yUmZlNTHxL3m/EhLNzuHNJ78lT9jRqcvx1fGbYNw==");
  // Blowfish based on [representation 7].
  test_serialize_x("AAAAAAgAAAAAAAAAYUjMnTbpeMgHAAEAAgA=",
                   "AAAAAEAAAAAAAAAAU2FsdGVkX19ccMFZDjIhjh8AlJ9UxwF6iW/v22OLbvdgO5A9lXfjiNOyTWKITkoDZMWovEaaFCTxMICx+VjwYw==");
  // CAST5 based on [representation 8].
  test_serialize_x("AAAAAE4CAAAAAAAARhodZUPoqySbIalZsCphbPWWx0y1FPEZApMOZ6DADOVaMgxHMDDhc/xqDxOYL9oJMKV9JP1SnODHZuPRolCSjnXwzfZNOXj/O4jxpeXAMqTg98nPIv67l6Fs7K3ggR9o+wNeySwNcjNh4ps5iMoxQ4PjBtkcQ7TOnxYZYpbt4sYT3RfxWZMTfJMBht/XXmM4uQ6xm83APo2xKgSX28MHCWzeFa+4JGAX/XSCMelrqFfiGXmbWmHrAyF7EmbqUytcAzg6Sm5oJ0uMM8fvkRlMVKQAG+OkOUq6wm7Kx+1mBOihxSIRfE7RppRS2RyYyrmbBNlAGxbMNi/9NdC2MXPJD9QrzwRna0Q7Ulwq2+G844HP1Kt7AsN3+Nys3uP6kYeCI2PoSuQUXFbM99qq9vn5A+aQV8MQQb8C3tffOKaCO0pEPes9s3q11YDdpZZCcZMwosgQ9gHWLanVJsqavqdi8pJLwflvFr71eI1Czl5Z8D/ZhPSGdpN68ZtsQGndGQhuQhhsDPrrhIqW2wOpI+zyvS0UHwZG6Dr55HS5OuVfS1aUDwx4Kc+V0zQgizukhveUhuO01vCbWl6Tm9TkQdM2ExBS58xCh88mgAWbnt3Nz5hHz2jPhSAfSCQ5c0vo7pEHGe2oT/MUXHigysbBJ9UrjHB3ktt9+GL0y1XgcNsbfinaNZiR1mBIyu2K8Z0YBJlDQoUWnh5aellJA5h9UgNUWX5nQbb9594FGCxkNTFKSwvkcPDMGTxpmjjmVQxX0rs8CjeAKr6mMBr1vlSbirYIAAMAAQA=",
                   "AAAAADoAAAAAAAAAU2FsdGVkX19JDwFg2cOr0WfM3BL9xpj+BseUsvsetlOd7hT7HonL2zOsqRS9GYUpwpUOpdB3ebpdgQ==");
  // AES128 based on [representation 9].
  test_serialize_x("AAAAAGcAAAAAAAAA/eShf52RTcdsqwS6ySPk7wsCbH5r7PWJg4sBAI6n6vfxlCTLv8BlnMftmaRal4WuLB/oxdBuYZoczvC6zAa+hucRMBpMFHrycn3Lo0BE0MEOzU3mp8gUlPYOEmKON4DDoTHsPRk0twkAAQAAAA==",
                   "AAAAAEAAAAAAAAAAU2FsdGVkX19RtYDVFjUDoFYTZJU1sGuoK66/MMHuoFAlmf+QE30rxlFY2xHTd+7cszCJVUUNUX1sgIi9THnihg==");
  // AES192 based on [representation 10].
  test_serialize_x("AAAAAHMAAAAAAAAAOOKm3nj4GxCgc4sigWw3TWN0alofAcpLlsjXEVODwjZS0rI2m/iIMjKYi9hi7yuLHmX4WF1WQk3Ag7CGDrKSyJ7B7tuZjKR7aWt4sZ0MQRQ7HX8SzSB1CEcLZFVgN0s2mjqUuBNsc5vvW3ESg5LPOXpInwoABAADAA==",
                   "AAAAADoAAAAAAAAAU2FsdGVkX18stpxDR0y0FWCadhaCVNMNzE78UYYWgX3TN2yAfi/OU6nbGUG4g/5GtM+I3WuKIPwrBw==");
  // AES256 based on [representation 11].
  test_serialize_x("AAAAABIAAAAAAAAAHtcyJIvNDkqy+8h23rxJ/KWqCwACAAUA",
                   "AAAAAEAAAAAAAAAAU2FsdGVkX18+IYWmcnMsaJLPSk0HvTWZUyt+HgnyF9qu5WxvWkfMd91+myPqbs4+ZKhqVv6LyUgax7U0r6vJjg==");
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
