#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Base64Format

#include <elle/Buffer.hh>
#include <elle/format/base64.hh>

#include <boost/test/unit_test.hpp>

#include <string>

BOOST_AUTO_TEST_CASE(encode_decode_base64)
{
  std::string to_encode = "";
  for (unsigned int i = 0; i < 100; ++i)
  {
    elle::Buffer buffin(to_encode.data(), to_encode.length());
    std::string encoded = elle::format::base64::encode(buffin);
    auto buffout = elle::format::base64::decode(encoded);
    std::string decoded((char *) buffout.contents(), buffout.size());

    BOOST_CHECK_EQUAL(to_encode.size(), decoded.size());
    BOOST_CHECK_EQUAL(to_encode, decoded);
    to_encode += std::to_string(i);
  }
}
