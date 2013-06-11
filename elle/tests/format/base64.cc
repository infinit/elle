#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Base64Format

#include <elle/Buffer.hh>
#include <elle/format/base64.hh>

#include <boost/test/unit_test.hpp>

#include <string>

BOOST_AUTO_TEST_CASE(encode_to_and_decode_from_base64)
{
  std::string to_encode = "";
  for (unsigned char i = 0; i < 255; ++i)
  {
    elle::Buffer source(to_encode.data(), to_encode.length());
    elle::Buffer encoded(elle::format::base64::encode(source));
    elle::Buffer decoded(elle::format::base64::decode(encoded));

    std::string decoded_str((char *) decoded.contents(), decoded.size());;

    BOOST_CHECK_EQUAL(source.size(), decoded.size());
    BOOST_CHECK_EQUAL(source, decoded);

    BOOST_CHECK_EQUAL(to_encode.size(), decoded_str.size());
    BOOST_CHECK_EQUAL(to_encode, decoded_str);

    to_encode += i;
  }
}

BOOST_AUTO_TEST_CASE(encode_and_decode_quote)
{
  std::string quote = "If my calculations are correct, when this baby hits 88 "\
    "miles per hour... you're gonna see some serious shit.";
  elle::Buffer source(quote.data(), quote.length());
  elle::Buffer encoded(elle::format::base64::encode(source));
  elle::Buffer decoded(elle::format::base64::decode(encoded));

  std::string decoded_str((char *) decoded.contents(), decoded.size());;

  BOOST_CHECK_EQUAL(source, decoded);
  BOOST_CHECK_EQUAL(source.size(), decoded.size());

  BOOST_CHECK_EQUAL(quote, decoded_str);
  BOOST_CHECK_EQUAL(quote.size(), decoded_str.size());
}
