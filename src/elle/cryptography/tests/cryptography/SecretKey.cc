#include "cryptography.hh"

#include <elle/cryptography/SecretKey.hh>
#include <elle/cryptography/Cipher.hh>
#include <elle/cryptography/Oneway.hh>
#include <elle/cryptography/random.hh>

#include <elle/serialization/json.hh>

/*----------.
| Represent |
`----------*/

static std::string const _message("I don't know what we're yelling about!");

template <uint32_t N,
          uint32_t L,
          elle::cryptography::Cipher C,
          elle::cryptography::Mode M,
          elle::cryptography::Oneway O>
void
test_represent_n()
{
  elle::cryptography::SecretKey key =
    elle::cryptography::secretkey::generate(L);

  // N.1)
  {
    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      key.serialize(output);
    }

    elle::fprintf(std::cout, "[representation %s.1] %s\n", N, stream.str());
  }

  // N.2)
  {
    elle::Buffer code = key.encipher(_message, C, M, O);

    std::stringstream stream;
    {
      typename elle::serialization::json::SerializerOut output(stream);
      output.serialize("code", code);
    }

    elle::fprintf(std::cout, "[representation %s.2] %s\n", N, stream.str());
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
                   elle::cryptography::Cipher::des,
                   elle::cryptography::Mode::ofb,
                   elle::cryptography::Oneway::md5>();
  // 2 DES.
  test_represent_n<2,
                   31,
                   elle::cryptography::Cipher::des2,
                   elle::cryptography::Mode::ecb,
                   elle::cryptography::Oneway::sha1>();
  // Triple DES.
  test_represent_n<3,
                   191,
                   elle::cryptography::Cipher::des3,
                   elle::cryptography::Mode::cfb,
                   elle::cryptography::Oneway::sha384>();
  // DES X.
  test_represent_n<4,
                   4090,
                   elle::cryptography::Cipher::desx,
                   elle::cryptography::Mode::cbc,
                   elle::cryptography::Oneway::sha>();
  // IDEA.
  test_represent_n<5,
                   2056,
                   elle::cryptography::Cipher::idea,
                   elle::cryptography::Mode::ecb,
                   elle::cryptography::Oneway::sha224>();
  // RC2.
  test_represent_n<6,
                   29,
                   elle::cryptography::Cipher::rc2,
                   elle::cryptography::Mode::ofb,
                   elle::cryptography::Oneway::sha512>();
  // Blowfish.
  test_represent_n<7,
                   69,
                   elle::cryptography::Cipher::blowfish,
                   elle::cryptography::Mode::cbc,
                   elle::cryptography::Oneway::sha1>();
  // CAST5.
  test_represent_n<8,
                   4720,
                   elle::cryptography::Cipher::cast5,
                   elle::cryptography::Mode::cfb,
                   elle::cryptography::Oneway::sha>();
  // AES128.
  test_represent_n<9,
                   830,
                   elle::cryptography::Cipher::aes128,
                   elle::cryptography::Mode::cbc,
                   elle::cryptography::Oneway::md5>();
  // AES192.
  test_represent_n<10,
                   921,
                   elle::cryptography::Cipher::aes192,
                   elle::cryptography::Mode::ofb,
                   elle::cryptography::Oneway::sha224>();
  // AES256.
  test_represent_n<11,
                   144,
                   elle::cryptography::Cipher::aes256,
                   elle::cryptography::Mode::ecb,
                   elle::cryptography::Oneway::sha384>();
}

/*---------.
| Generate |
`---------*/

template <uint32_t L>
elle::cryptography::SecretKey
test_generate_x()
{
  elle::cryptography::SecretKey key =
    elle::cryptography::secretkey::generate(L);

  return (key);
}

static
void
test_generate()
{
  // DES.
  test_generate_x<251>();
  // 2 DES.
  test_generate_x<120>();
  // Triple DES.
  test_generate_x<12>();
  // DES X.
  test_generate_x<1281>();
  // IDEA.
  test_generate_x<319>();
  // RC2.
  test_generate_x<3016>();
  // Blowfish.
  test_generate_x<812>();
  // CAST5.
  test_generate_x<317>();
  // AES128.
  test_generate_x<918>();
  // AES192.
  test_generate_x<124>();
  // AES256.
  test_generate_x<1083>();
}

/*----------.
| Construct |
`----------*/

static
elle::cryptography::SecretKey
_test_construct_aes128()
{
  elle::cryptography::SecretKey key1("chiche");

  elle::cryptography::SecretKey key2("chechi");

  BOOST_CHECK_NE(key1, key2);

  elle::cryptography::SecretKey key3("chiche");

  BOOST_CHECK_EQUAL(key1, key3);

  elle::cryptography::SecretKey key4 =
    test_generate_x<128>();

  return (key4);
}

static
void
test_construct()
{
  // AES128.
  elle::cryptography::SecretKey key1 = _test_construct_aes128();

  // SecretKey copy.
  elle::cryptography::SecretKey key2(key1);

  BOOST_CHECK_EQUAL(key1, key2);

  // SecretKey move.
  elle::cryptography::SecretKey key3(std::move(key1));

  BOOST_CHECK_EQUAL(key2, key3);
}

/*--------.
| Operate |
`--------*/

static
void
_test_operate_idea()
{
  elle::cryptography::SecretKey key =
    test_generate_x<512>();

  std::string const input = "Chie du foutre!";

  elle::Buffer code = key.encipher(input,
                                   elle::cryptography::Cipher::idea);
  elle::Buffer plain = key.decipher(code,
                                    elle::cryptography::Cipher::idea);

  std::string const output(plain.string());

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
test_serialize_x(std::string const& R1,
                 std::string const& R2,
                 elle::cryptography::Cipher const cipher,
                 elle::cryptography::Mode const mode,
                 elle::cryptography::Oneway const oneway)
{
  std::stringstream stream1(R1);
  typename elle::serialization::json::SerializerIn input1(stream1);
  elle::cryptography::SecretKey key(input1);

  std::stringstream stream2(R2);
  typename elle::serialization::json::SerializerIn input2(stream2);
  elle::Buffer code;
  input2.serialize("code", code);

  elle::Buffer plain = key.decipher(code,
                                    cipher, mode, oneway);

  BOOST_CHECK_EQUAL(_message, plain.string());
}

static
void
test_serialize()
{
  // DES based on [representation 1].
  test_serialize_x(R"JSON({"password":"4qB6U0hb4UmdtnsQr13y"})JSON",
                   R"JSON({"code":"U2FsdGVkX18V6o42EqPDOb1qiK/b9W387PK4o3R/gB9MpMQlAu4tQDKOWzrsCNBrhEDcYhk4"})JSON",
                   elle::cryptography::Cipher::des,
                   elle::cryptography::Mode::ofb,
                   elle::cryptography::Oneway::md5);
  // 2-DES based on [representation 2].
  test_serialize_x(R"JSON({"password":"FFtR"})JSON",
                   R"JSON({"code":"U2FsdGVkX1/ROIWQ8DGP0oM0NgTvb7EOhnNeeiOtA/mxf4pagRj2FFmUVgOLErdMsVon7qcD1bs="})JSON",
                   elle::cryptography::Cipher::des2,
                   elle::cryptography::Mode::ecb,
                   elle::cryptography::Oneway::sha1);
  // Triple DES based on [representation 3].
  test_serialize_x(R"JSON({"password":"q0k3yuZAGrIFnOELwsqzusnpzs+l3no="})JSON",
                   R"JSON({"code":"U2FsdGVkX19ww/Mytu3drPqHTDC2fPhPTDkrJ7y8Emk6dEmVSwxI4LrKzWhaGIEC+gdQXc07"})JSON",
                   elle::cryptography::Cipher::des3,
                   elle::cryptography::Mode::cfb,
                   elle::cryptography::Oneway::sha384);
  // DES X based on [representation 4].
  test_serialize_x(R"JSON({"password":"NGU1k9R8VR3vQeJO9CbI0SytThq6a4Vvoz6qRvJ11p0MuXv9XRduY7tO9DcIJImfx8ZslNGdChqczObQ5HlkCPr96aOPY2h3/0Xas1MYy/DQUZSHI5C1p56bFlEXhY9BxCNC2w7nNkjA2T+wHlXMxzCQCt4aRlRVhMBgi16hpHGTUwS3luk7L8b0rSdNan1HYIsnrBRVXIfb1gAR7d1B+fE1IM+9PLRbl7hhJ8fhk+KpAu7qyfh/Hd5v+6h1vsVfLFwv5k+dPUvEz64/VxxNxAXtj0jPgUgVHJM+ZJfiSZ17nJ02vFc0wUpXROQ8TLEc7X3YVwST5uLfwxQtqlFXujDoXNIxO8UYOKDTsQvKzMhDypchUxwg5xX7/Migr3E2u0VG7aqlZ0HYPnuezE5wThryaIS8EBrXfhxG8axgrRq7PLe9otEA/8DlTsmaQ3VmwIEAJaT2G7UdXMp31yqLv0qr879iuKm/czutnzkQhdkMM/rb52JzCngUS8Qsx4rgJ1HY5eUE+9jZJt5SQDdpckxlA07J9BqPuE/XGMdlKiRzNiX5L2+WOvADPjYGsxCQIQ+WvY7Gbc0ez3qMOTKmHdFSbi7aSX+4BeLwxAGcX/31L2nw7L21VSEVW+81Kn7x+fGQkC44Ahj/3Y6hGkfQvKLUDEuuFABtxzRrs+kPpA=="})JSON",
                   R"JSON({"code":"U2FsdGVkX18i8qa4hvVnpeZRq9DxUEo4qV6NLIEv1kfuYnSuXr2O863ODv3qO4XDtmkUdHWjBl4="})JSON",
                   elle::cryptography::Cipher::desx,
                   elle::cryptography::Mode::cbc,
                   elle::cryptography::Oneway::sha);
  // IDEA based on [representation 5].
  test_serialize_x(R"JSON({"password":"bx5Ll6CAhp8gxu86nHAjaeuOx4eDhvSwRbH4FaIDLxLR/OBsn7dvfaKpNrT/KvumQeCqJ8XHDBxxG4/qYFogaehUT6pkQy23sMwsLoSApLEdv9/X1qqDpohBwAHjOuHcab5qTiLHvAJHgBryxoWMUJ6RNj0NIVcsA2aNO7o64N1IIz/+fhNXVFs9yJXrRuWEJ+ymDiACHnXq83hQ+1O7i/mIbKghsWXmdAHDyXQDk0KNOKmkw/oqgaACTR2YAsDQDPntSOsOP/fMbGfUpvGujjnseCREY4SnZ+b5UwTdm907ZXP9HiDCOReElgBn55lUryd/UQwZ/AKzCxtdmV3PbqA="})JSON",
                   R"JSON({"code":"U2FsdGVkX1+UHeTX6vv3TdbyKsA3js3Gu9cPZgPCeWJ4GuitSrB1LpYGqkIboZGe4PeOVBrbeHo="})JSON",
                   elle::cryptography::Cipher::idea,
                   elle::cryptography::Mode::ecb,
                   elle::cryptography::Oneway::sha224);
  // RC2 based on [representation 6].
  test_serialize_x(R"JSON({"password":"dMX+"})JSON",
                   R"JSON({"code":"U2FsdGVkX1/NwfbdPzil23ySr9WnGkPTRnVLZhByYStupF9/dEP0iG3WC9UHyXc9e5LWVlHM"})JSON",
                   elle::cryptography::Cipher::rc2,
                   elle::cryptography::Mode::ofb,
                   elle::cryptography::Oneway::sha512);
  // Blowfish based on [representation 7].
  test_serialize_x(R"JSON({"password":"CGODeAH8Cms="})JSON",
                   R"JSON({"code":"U2FsdGVkX1+FuogLtF7wVT8bUDRq5W1kxWr+fc8vkOUqwKVYutvzZtD89+xKp4MkvFUx4NwFUpw="})JSON",
                   elle::cryptography::Cipher::blowfish,
                   elle::cryptography::Mode::cbc,
                   elle::cryptography::Oneway::sha1);
  // CAST5 based on [representation 8].
  test_serialize_x(R"JSON({"password":"LQauVO6pBtFgr2gMR1eH8MXkw8GUJkV343tmjyVC3Lm8dSPbkjGZj01Is7CeMPR3o3vt8cewFXik3hrRpQW6EwWpqtoLCyXOxmm2eZg9q3PcC1fJpYEQrbjGp4YUZO1k+OLzZw0XB704rYnYWgC5t78rR7pmlxs8dy5AdA4CLfottrCLHx7sZWGd870uhT6ckFF5DXJNLLCrojaa/JbssT86PLKTW2GHBkqWcPn3fKGlrSDUMNNnZk4/KV6MiZAJyGAmP8tezsR9Qxx/6fM0ur8zy1o/3YuV/tJAtRlcrFit7ZYPoYqILG/gP2VPTFOTrAOgMyUe5ZEAVcNgo3ukFpTrnd12a4VjOsmdWWMagLFt7uruCJk1O+AhzAgrE0l5i9/azTcIPVIpyOsIBk1aN/N1atq4EG4VT4PUA1e/dZX1+VAeFzbzElwabhzNepL5bmGAGqVWhkPL87HZEbw6Smj8EPjq0GjOM6Z0iXVnxNCEpFuXYbNlwU0rFs2RfQonYPxYErXCyoMfh9KMhFthb0rO0+D3XDxFIJB7u7xKmB3ugrbys2cGenRcvFuJ1Bhoanp265DrZ41OKEJhRVCoI4trw7jGQa0xcAggLVHR4+hGV+KPy8dCt2oKmVLzaXN6JH3U0f0XgQUgJJvFLKFDje3Dw4SX629iAuglel8tu/V7LbgJM3B6r7InI0ixX0tQkGoZ8W3V7ND7/f6abxZ7mN5mOQVfDTj/coB0haLYoxigHK/McXy8n23oUwj5v3zMFMmvxK+A0mR+mFdjB4M="})JSON",
                   R"JSON({"code":"U2FsdGVkX1+0TJWw1NW6CfkREOcHr5FRZDiE9qk6KREVI10NmQkg0SmSV9GfTcoQc+t66IiM"})JSON",
                   elle::cryptography::Cipher::cast5,
                   elle::cryptography::Mode::cfb,
                   elle::cryptography::Oneway::sha);
  // AES128 based on [representation 9].
  test_serialize_x(R"JSON({"password":"3QNCKRA0/q+alW/kQnHZ1/2smtl6yAFwu8balzJgvy0x5/ppMJaCbvr/I0/DObUP/WXjGrdpPCg/vGK5lpCS2jzlsImNuBwZUeJCn5IOsGSXgEVw39vuT6gbuhKRoe85+86ibLZ0Hg=="})JSON",
                   R"JSON({"code":"U2FsdGVkX19VjZi/JfEzFXB7u8Ll7ZnedxvM0F6Ns0UcUve7TI/XajsT0mDYePnaw+LrdgghM5aSiZDC5uwtUw=="})JSON",
                   elle::cryptography::Cipher::aes128,
                   elle::cryptography::Mode::cbc,
                   elle::cryptography::Oneway::md5);
  // AES192 based on [representation 10].
  test_serialize_x(R"JSON({"password":"KmvsgwIWGDy9tomxayOkZ/TP6fmXPEkUiX8QvNy52BSs/+s7jQSRS8H5jaGIfg6xCwDgN7fJ41z38oSfOJ+1fmeR0BiE/pP2OHO0a7+8xDzAldKHt5sbfpwfm20z6d3DmNiLG8FFf1h9wTjoos2JtUdXyA=="})JSON",
                   R"JSON({"code":"U2FsdGVkX1/p7NMq8KcJ9zXZYAZ6ReP9hmzlTNMs/CRSIiYC0YahwvVp6N/c71Hx+stiTrGf"})JSON",
                   elle::cryptography::Cipher::aes192,
                   elle::cryptography::Mode::ofb,
                   elle::cryptography::Oneway::sha224);
  // AES256 based on [representation 11].
  test_serialize_x(R"JSON({"password":"tNKK2OlijngOIEut27AuGo3w"})JSON",
                   R"JSON({"code":"U2FsdGVkX18iZy7p279VN6KalsQqH5sTH1i2tcTj7V8wFGxFncRkNE617uB7b2EvyJ0ata14BYQV1u9fRsqCUQ=="})JSON",
                   elle::cryptography::Cipher::aes256,
                   elle::cryptography::Mode::ecb,
                   elle::cryptography::Oneway::sha384);
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
