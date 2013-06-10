// #define BOOST_TEST_DYN_LINK
// #define BOOST_TEST_MODULE JSONSerialization

#include <elle/Buffer.hh>
#include <elle/format/base64.hh>

#include <boost/test/unit_test.hpp>

#include <string>
#include <iostream> //XXX

// BOOST_AUTO_TEST_CASE(Format)
// {
//   std::string to_encode = "I'm a beaver with a flat tail.";
//   {
//     elle::Buffer buff(to_encode.data(), to_encode.length());
//     std::string encoded = elle::format::base64::encode(buff);
//     std::cerr << encoded << std::endl;
//   }
// }

int main(void)
{
  std::string to_encode = "";
  for (unsigned int i = 0; i < 10000; ++i)
  {
    {
      elle::Buffer buff(to_encode.data(), to_encode.length());
      std::string encoded = elle::format::base64::encode(buff);
      std::cerr << encoded << std::endl;
    }
    to_encode += std::to_string(i);
  }
}
