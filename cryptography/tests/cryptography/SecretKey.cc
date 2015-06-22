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

static
void
test_serialize_x(elle::String const& R1,
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
  // DES based on [representation 1].
  test_serialize_x("AQAAAA8AAAAAAAAAgRKSfrv7dcIvOviBNRt7AQAEAAAA",
                   "AAAAADoAAAAAAAAAU2FsdGVkX1/covAbhggD4IvRV7f++2/IlV7dEGh61c06eMV8q7Hf5g3oecy9OCaSJSHvgbK8fRpgjA==");
  // 2-DES based on [representation 2].
  test_serialize_x("AQAAAAMAAAAAAAAAuz7dAgACAAIA",
                   "AAAAAEAAAAAAAAAAU2FsdGVkX19XYFP8itd8ey0klWDRE0Y2pxnNgVkQWgQTG90cuphi7B4gVYFXvMTW6scyT+gE8dSv5rnTqYaFpA==");
  // Triple DES based on [representation 3].
  test_serialize_x("AQAAABcAAAAAAAAA+6TSXsyqUW8TFuv4We+qSeerBPGOKfADAAMABQA=",
                   "AAAAADoAAAAAAAAAU2FsdGVkX19hnfKXeA8pXmKv9ES0xNPObhOOM5QvX3t4x9c0RxG0LEekFT4YEQ4afL7ZQ7UAppVX6w==");
  // DES X based on [representation 4].
  test_serialize_x("AQAAAP8BAAAAAAAAqolxdsot8TNgBmoIDoiXpfaoYaUjgYgXwIUwRCppYzoxvYayLt3a9DekiBxMXRrimvLxKScQABNtM2XyYiRraEfccnl+3qrlE0igu7v9tD2vwz29dRll2tHWIiMKokkEec/8xGcrboGzVkIt0F+t/GXO7bTGgcjfhsCVqi/vd9WQZLmMhsMm6d9ZV8yujhDSbfotMzxbKMBbG7+VIASKa/ixZ5c3X61GzWLIxmu+4wWiHsoAVHkOewcTnFwOEc84gfsJxm8Y+B9PusJMOnJqBe3087r/gmtmVZiWOA54pQ1UBf/yGqjFOV5MgFAc9yRUBvHpOIVC3D9kfCnvvikgKC52qPloYxIU5r0B8njNwJ2kjivxShSax53wTCftFfbuepHAsHaJJ2FzIabvt7ByVAcD+ddgKpgfKCuYM7PkubYzMnRU2N5NhO//YWJ+oY2svFP5cdEhiwF5hWbK/GyfWDt+vv7pM5Kw8Hljq88EcZFaKv5UksnEsW1qV5ZgZ91qIqXOU2xEFNS96gt+kN2HNiJYI3/GiFp2MTO58FxI7duvcBTKPB8wwVglEeBVyOjtUrDmCUdeC7wz9GpzjYVfONMBO3nSArxmGPtEIDtSF50/KCPq5p7GgXJt/4bUXmfmGmk6GkH5tsf8bnA3zDZ5A3SExKAhxq5u74Oo3xTjZwQAAQABAA==",
                   "AAAAAEAAAAAAAAAAU2FsdGVkX1+LO/a2oO427jkHvodl7rtn3vkBRIghvKaj00amld8c3JN+TtBdbpCBlLqVUR6ly2T9vToTvfHKIQ==");
  // IDEA based on [representation 5].
  test_serialize_x("AQAAAAEBAAAAAAAA0QEkZ42azav9YWVfjpfyV6v5sf9rDeo4eTiwj8739Avm7nlo1gPI+O5TVpKAzWlAzEjzkiyjgVWjVTQdAHpH6KmSAeMeOIA+WzZ+LMaMzMyPRVCDsRsOVf8Db1pH4EoNbQVw62aFcCkiXMo7wrDSuhx7Ov1t5fpvyqI/y1r7kMGHXDoVWnPz2+/sZaYs3OFbFUDNTdJ5N4zR4UfHuu3pP6BpD2NWGKAKYxOQR1wuNBLkDiudWQ8WhjKS3zXV9mzirw/2kKHFkcsgOD3YfnNPdN65EdxnS1EnC3dJA7BSko8xRtHTgqcxUspDSfPsxvq2a78LxyK7KeRluGu31cw0288FAAIAAwA=",
                   "AAAAAEAAAAAAAAAAU2FsdGVkX19BcRcDzaeNqWSMkTsYMBnfPBVs+gHszUyft8kBdxNUDxXmhkTtgM6ffzL1vEdzVm2fRiBuL5aLHA==");
  // RC2 based on [representation 6].
  test_serialize_x("AQAAAAMAAAAAAAAAoW28BgAEAAYA",
                   "AAAAADoAAAAAAAAAU2FsdGVkX19suoqAaBTGWm/FPIGtp3fi821SO1Q3RHWHoCkjFsjTJ0wCoKRi2thgZ5Z7ke3ia6IjcQ==");
  // Blowfish based on [representation 7].
  test_serialize_x("AQAAAAgAAAAAAAAALPc0McD6h54HAAEAAgA=",
                   "AAAAAEAAAAAAAAAAU2FsdGVkX1+ithCbTdvjDbB+Fpazk9laiPeD9yQl2nCzl9lfqcvmdlqMXnTWeu1Chy8DJ/0QRF/pjD+jpUFaiQ==");
  // CAST5 based on [representation 8].
  test_serialize_x("AQAAAE4CAAAAAAAAHEUfxgp0G9n3tdGv7MIO5wrK1GqfjpNOzlTiIrgj2RtFC0MORn+YsZIx4Zb/S9ihLp1GDsASavGq9SQAvSIczZyDR0KbUxEIfRZKPvtTaL6zZcR4U0gW03CQH2oVCwgxg5zEg8cdp9jrTHbJsYOK0iElF5a8rUHOqxiEDtiTOHkSpGZPpgChIS/88uKbtnCmuwupC0PzlOjUB3ZsctiRcxkcldVOLReLNdjFxMqDYuVAS64boghd+0exsHcnb5X/GV9EEeF5q7P8JnENt5LQ0rbveppppYAFcF8aPYADTNhHslKLz9qCoYwu48tCR30cn1LVuyw5E+9uEhBbRiVGWbqYO4FB2WBHhaNXxAc8oiXpO1ITrJTEjuDUp+HRmV5TVtXClFIeAt0GJhLeCpkgUYSS9XOEyrHF0Hq1QCsPWM51v5yK5ZMRg2oz6VeHYGjuelhWKuEhM8p6ko/W3dl1a8I5B7ltdGrXZggQ9MVjrQoneJ+t90gtCg7e5rerRfbas/FKCHnXlvAjCnyeKUD63sLDbfvxnyxVIQSlSgE4YMIHYyC4aVcg/VaomwGwW2/cYbyvhhpsnFttcwAw2Lxh8VuYNqz6mtVgS1rS6ZeD9j6Amv/VspiII/3vFo2N2NDCv6a1mcyXBW91ELLo36x97sWWHp9kniIrJbSctgNIlMrAHPI41HPMIeldYdwie0l3QSJxh6ZMdSfcV6978EkdoJOm2B9sfq6rN4WnOJXqM1+dQkjb0J+8UEN9T+iOguBqI0e5ZCQa4DQ8MeqoQyMIAAMAAQA=",
                   "AAAAADoAAAAAAAAAU2FsdGVkX1/0LI4kKFVZAsTE5qtvzhwQKhaBXfSs0XnZiRrBmKiGZwXVHUfrWGEI9ZVS7aU2sFvx6w==");
  // AES128 based on [representation 9].
  test_serialize_x("AQAAAGcAAAAAAAAAgBBmom7ShMu5z23Iv1v4PPYgDp2POgY0vOhLQa0+IbaSNz8xDg6PEhW7Bf2Vid6tumBAzIxLaBnuUHI4TTNW8qknSMSBOlrPsIE48kvvt4i7UIZR4SqHf411eHRqj73O6YLO8/PMjwkAAQAAAA==",
                   "AAAAAEAAAAAAAAAAU2FsdGVkX18GwuLUHh9wARTox0O/FA1GHRy6gUFUBtF2Dd0/O0WoujqEt/VqZ9f/1pm0dOR9qrNa7v4QMoTWAA==");
  // AES192 based on [representation 10].
  test_serialize_x("AQAAAHMAAAAAAAAAEVenRzR41PCt83zF0kzddEOK7DgG8G/Bqz+FrBU7Wo+nmv7/GRd9GcpMJhdn4ISXZ2K4XAvdENyJwr5rMfMQeYG/uurPvxrTdtNVqsJgItwnM2dsuJSxvM5jcZs8VS2oPvwPvtwsrmkJmpI3XBoxHcWaDwoABAADAA==",
                   "AAAAADoAAAAAAAAAU2FsdGVkX182cNlusZDOb97+MAlOE6CKXZm1V469Hp37mHVCXF5ypBZCO9Yee4J4gpWEOitDhPpCEg==");
  // AES256 based on [representation 11].
  test_serialize_x("AQAAABIAAAAAAAAAKBJFnW8oPO1WaSig8CeTrkyCCwACAAUA",
                   "AAAAAEAAAAAAAAAAU2FsdGVkX1+XSmhx5v23fJrI1OUfSUhr1GBRKXAn6qjU1nsEeb4VJJfBg6l2ID8UkFtTa+PhKVZQOyMtCnNSZA==");
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
