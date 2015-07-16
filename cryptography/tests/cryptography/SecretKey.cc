#include "cryptography.hh"

#include <cryptography/SecretKey.hh>
#include <cryptography/Cipher.hh>
#include <cryptography/Oneway.hh>
#include <cryptography/random.hh>

#include <elle/serialization/json.hh>

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
    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      key.serialize(output);
    }

    elle::printf("[representation %s.1] %s\n", N, stream.str());
  }

  // N.2)
  {
    elle::Buffer code = key.encipher(_message);

    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      output.serialize("code", code);
    }

    elle::printf("[representation %s.2] %s\n", N, stream.str());
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

  elle::String const input = "Chie du foutre!";

  elle::Buffer code = key.encipher(input);
  elle::Buffer plain = key.decipher(code);

  elle::String const output(plain.string());

  BOOST_CHECK_EQUAL(input, output);
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
  std::stringstream stream1(R1);
  typename elle::serialization::json::SerializerIn input1(stream1);
  infinit::cryptography::SecretKey key(input1);

  std::stringstream stream2(R2);
  typename elle::serialization::json::SerializerIn input2(stream2);
  elle::Buffer code;
  input2.serialize("code", code);

  elle::Buffer plain = key.decipher(code);

  BOOST_CHECK_EQUAL(_message, plain.string());
}

static
void
test_serialize()
{
  // DES based on [representation 1].
  test_serialize_x(R"JSON({"cipher":1,"mode":4,"oneway":0,"password":"qExjMaMn9dtbg3qzQDvn"})JSON",
                   R"JSON({"code":"U2FsdGVkX19BJmEt9cGFrHQEPoDofWmigVSJgUX0+izgvU6Ay6DFrJ9Sywcdu+yWIP3H3QFc"})JSON");
  // 2-DES based on [representation 2].
  test_serialize_x(R"JSON({"cipher":2,"mode":2,"oneway":2,"password":"Ph+3"})JSON",
                   R"JSON({"code":"U2FsdGVkX1/w0okCpBBhjauVH2800c8fLMuinffa0WbEfebJOZmrnXHSieIE4wU6xHgdj+B9xEs="})JSON");
  // Triple DES based on [representation 3].
  test_serialize_x(R"JSON({"cipher":3,"mode":3,"oneway":5,"password":"7tJCWuLp5HbwZu54Kfv/fyBXT4Agxl0="})JSON",
                   R"JSON({"code":"U2FsdGVkX1+A3z6bu04s3mvEoBmyK2ooTZVBTlgbEUGamVIY8NiABYznOd/t7lBfBiAywyve"})JSON");
  // DES X based on [representation 4].
  test_serialize_x(R"JSON({"cipher":4,"mode":1,"oneway":1,"password":"AefWhYcagQEHGLI/VB9xk4jjEBiOzXiqlvDcsjdIOsZaK+CuBN98EhHf5cSc0G1HNgQJlzck+w2cvSIPuMInlk5y6CMWIUQwVVDjCN/HUMhSVhc7h0y3TOOy+y/lFNRbPP+mWfV9slroea5doRNXkakBOjR1VpXZQMFh9cxwaISjnYbkRhmNu0qytWEJmuMQmn7FRBNF/LuGqBrExSMRmiJe3FYUp09RHIySQfmwxnr860/pd4L7RQpNgfZI7P8mpvcEhBKE2v13sPNY3iPOuVzFFRA6zikYUVIbcoXhO/bSOWlQF0eEEF8eCenQjWpgdyFV0jf49mELJ5YoK0JFiwfp5eqfLUREm1DWX3VdI1/9GSh+zbQev71olVdXRjvL5Y9qR61jccVWt6R5IXlyFJ03XCfqhljKCfbvK0y1XN1Ib4wtMv5sOZ4aXZyXojJE6q5TASyLtm2WiEpuZK1h2/a3Ca8gq+RjmVYoB84pIGQQKdgi3/4/sOwR34EoxES7lO8OKmdVxoxR44HYtMyxDNzaUOWcIYpyrG5T5rl8fmtuI5mclraAPzNask+VSn4BMks2iQoa6ruyhQrr5bRBX9OMbYLNfpIK4Us99Il2wBGT1fqOBMw/hKAHdprfSzYfaude7ASncHcapHuRnNYrWMh3+gvh1UyD18m0v6RoBw=="})JSON",
                   R"JSON({"code":"U2FsdGVkX19mp53BhfFOUufvpDUYph59t27CnqCgATQ1SBoyqHNsLfI1ZOXID8JUkQ0+khEQ9Ys="})JSON");
  // IDEA based on [representation 5].
  test_serialize_x(R"JSON({"cipher":5,"mode":2,"oneway":3,"password":"jpRAwK/cmaccGdhdDU6CZRvvnJqz8+EMgKGeml1TQ4Mc8o9bDAHliLm0Z9JGizlnOlkrrNu7X3Luo++2S/Y/0QCgPMO4z7P+8n82VhesUGueYfplnIUxAw09q11wqXo+0eOKjhrovpgfEx77JULwXP1Nm64Yd7Oq8k2JYuMPYYyV0SIwYP1eXBf5Cxuoj//38cuWXzx6YkBAvmnAG0Xy+ywpJIvVBjgGnQ0yddlMl8HZKfeY8Ce02/RlRB9boDvXQzwGyDMp/N1k1sUggRDqyfHDTea21+GNtAGVKM8hIe1bpI5fl5gxclmp4xXyEvsPF6yJokcedrCYd3x4K0XxsCQ="})JSON",
                   R"JSON({"code":"U2FsdGVkX19RAulp33SqeVl5elk0z1rmBhvwQBAaX+KITT/ZErMnzpqlEwDIhYDuDJmSiPC4JqQ="})JSON");
  // RC2 based on [representation 6].
  test_serialize_x(R"JSON({"cipher":6,"mode":4,"oneway":6,"password":"jXTY"})JSON",
                   R"JSON({"code":"U2FsdGVkX1/gbxUcPpazj52DwCzgij3GAm5c5LZ6xbON+h9/WqYJ4tAVWc9B1/AY2Tq8HEof"})JSON");
  // Blowfish based on [representation 7].
  test_serialize_x(R"JSON({"cipher":7,"mode":1,"oneway":2,"password":"N6rTUe3HCak="})JSON",
                   R"JSON({"code":"U2FsdGVkX1/7flbuLH+RGgIy19goe3mlUUbeoH2AsDhK/yZcG0J+kBohGYPDwV+W7kGjdOx8+3E="})JSON");
  // CAST5 based on [representation 8].
  test_serialize_x(R"JSON({"cipher":8,"mode":3,"oneway":1,"password":"rV3rzCsVYTjfi8WlNxOtKYGOuHw+IWlPFpJIMV0i7O71QCWfEJD4lsvhyNFWnn2tVw7HQwi6iI/BmWCtRfATNmgMuoQ/GSHzUSOu7SqBDzJVT+WL0Act7QEPXKEqRhny55PASkzq6Xsax8alxBiGeXcsfUOArUH4Yx05Txir/GmTioWUUGZURWjPuj23Q/mOXNh9bf9lYMO/Wy84iDYhXs9U5E94iDly0m2O22o9E3YOIDTnCrt3zwxo9MNcd+ROJLCXS8UUcSiB59T/zwd20gpkE/pCpuo5lv370VD7edRYLBH7ggOyX+XtdM7TbLKLVv/D7pEZ8k6jots/TjP7wArmGcGf/LffMDwX+DTfaPAE1cIYFgi4YPgtxGZnrFnDFOSOz0rmhPASbSFIh7P1mm2zzQZGWe8Ib8ZC9uJjq5tgHkuMs1SzNEnsgbkBZy62sOKyf26pWiX0Fl5Ttat+5fff1yc71z1uWd/v2mM25RMX0yj4sBDZt1JzrsC8/QlmFbUj9H4j5FH35JYVLvf2VTymg8NZ2e8DfrHdaMwZPluQGuKobU18CG7o7Zw26GSAPdnVCYAL36zLGjWqrTjgXYY8OnYSm3ZAzz4Hd8DoPwuXs7ao8g9ZLp13VZ9uk42ZFo4ejKHZ2EnlEKWd4voMbdPey2l4emUg0ff5yhkMWPy/ykmqiuaKvwTfBRt2+/cHLy2YMCyq8zeK7nr1aEkm2gc/2i5MydXag2mzpYla+l0oGawoj+lp0CpUtGjtYpL03218S0l6ksVhy8dYcWg="})JSON",
                   R"JSON({"code":"U2FsdGVkX19zCtln/H3DU0ihatOcBMgu0oPafHwNmSG1Fluwe7+wQmJEtLK129FNkFbxQX72"})JSON");
  // AES128 based on [representation 9].
  test_serialize_x(R"JSON({"cipher":9,"mode":1,"oneway":0,"password":"pRJ57hEE2Fvkt+RM1isUtVvDL7KNqgv7KF25QMkm+NSwHQVFzhduBLvsNPe/fCeT04q5k6sdByGQ/dCqN8NI56wNs33+YuFv1xGwbo3EQZzHsmJ35MlWgl9dnbMPdSVLMr2sPk4MKA=="})JSON",
                   R"JSON({"code":"U2FsdGVkX188pjfzbZuEtLMTDottclYaKWU+zXEU5ijxhk0ZUnurRN0kAdRelPyyUbMhzg1qmO4JuT84PIsrwQ=="})JSON");
  // AES192 based on [representation 10].
  test_serialize_x(R"JSON({"cipher":10,"mode":4,"oneway":3,"password":"k1yiX1a0hkpnnoe2ko6jq/HNM6OSJHAcfzQEPKOLRdJPJW/cWvQFCrlECOLaImqi7ouiVtj9oBox/mfdm6ZFXC7cU9xIoOk8wts5IUFLFTAvR3TYGX4oez5Fh9w2VEdLTg95e8PF8RZ3WUDitxHeEeIOkw=="})JSON",
                   R"JSON({"code":"U2FsdGVkX19QfjndNx+8rDjLXSjWa3BqeuqzY0HHxxibR99031wTuugCtXO36pG8Ds2/Kj8O"})JSON");
  // AES256 based on [representation 11].
  test_serialize_x(R"JSON({"cipher":11,"mode":2,"oneway":5,"password":"MLG20WdnAOQA9h0cbnqhDCK7"})JSON",
                   R"JSON({"code":"U2FsdGVkX199hXW/lN1gytAiR6YVopESdZ08lYPpNC23RolSBdZ56prRgYPT4PQTzNhS1u2YvJHf5cCNyDO/zQ=="})JSON");
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
