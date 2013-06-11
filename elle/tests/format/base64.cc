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
    elle::Buffer source(to_encode.data(), to_encode.length());
    elle::Buffer encoded(elle::format::base64::encode(source));
    elle::Buffer decoded(elle::format::base64::decode(encoded));

    BOOST_CHECK_EQUAL(source, decoded);
    to_encode += std::to_string(i);
  }
}
