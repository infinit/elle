#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Base64Format

#include <elle/Buffer.hh>
#include <elle/format/base64.hh>
#include <elle/log.hh>

#include <boost/test/unit_test.hpp>

#include <string>

ELLE_LOG_COMPONENT("elle.format.base64.test");

static std::string read_string(std::istream& input)
{
  std::string res;
  char buffer[1024];
  while (!input.eof())
  {
    input.read(buffer, 1024);
    res += std::string(buffer, input.gcount());
  }
  return res;
}

BOOST_AUTO_TEST_CASE(streams)
{
  std::string quote =
    "If my calculations are correct, when this baby hits 88 "
    "miles per hour ... you're gonna see some serious shit.";
  std::stringstream source(quote);

  std::stringstream result;
  {
    elle::format::base64::Stream base64(result);
    {
      auto const chunk_size = 16;
      char chunk[chunk_size + 1];
      chunk[chunk_size] = 0;
      while (!source.eof())
      {
        source.read(chunk, chunk_size);
        base64.write(chunk, source.gcount());
        base64.flush();
      }
    }
  }

#define BEGINNING                                               \
  "SWYgbXkgY2FsY3VsYXRpb25zIGFyZSBjb3JyZWN0LCB3aGVuIHRoaX"      \
    "MgYmFieSBoaXRzIDg4IG1pbGVzIHBlciBob3VyIC4uLiB5b3UncmUg"    \
    "Z29ubmEgc2VlIHNvbWUgc2VyaW91cyBzaGl0L"

  BOOST_CHECK(result.str() == BEGINNING "g==" ||
              result.str() == BEGINNING "m==");

  {
    elle::format::base64::Stream base64(result);
    BOOST_CHECK_EQUAL(read_string(base64), quote);
  }
}

BOOST_AUTO_TEST_CASE(encode_to_and_decode_from_base64)
{
  ELLE_TRACE_SCOPE("encode and decode generated strings");
  std::string to_encode = "";
  for (unsigned char i = 0; i < 255; ++i)
  {
    ELLE_TRACE_SCOPE("encode and decode: %s",
                     elle::WeakBuffer(const_cast<char*>(to_encode.c_str()),
                                      to_encode.size()));
    elle::Buffer source(to_encode.data(), to_encode.length());
    elle::Buffer encoded;
    ELLE_TRACE("encode")
      encoded = std::move(elle::format::base64::encode(source));
    ELLE_TRACE("encoded: \"%s\"",
               std::string(reinterpret_cast<char*>(encoded.contents()),
                           encoded.size()));
    elle::Buffer decoded;
    ELLE_TRACE("decode")
      decoded = std::move(elle::format::base64::decode(encoded));
    ELLE_TRACE("decoded: %s", decoded);

    std::string decoded_str((char *) decoded.contents(), decoded.size());;

    BOOST_CHECK_EQUAL(source.size(), decoded.size());
    BOOST_CHECK_EQUAL(source, decoded);

    BOOST_CHECK_EQUAL(to_encode.size(), decoded_str.size());
    BOOST_CHECK_EQUAL(to_encode, decoded_str);

    to_encode += i;
  }
}
