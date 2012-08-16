#ifndef  ELLE_FORMAT_HEXADECIMAL_HH
# define ELLE_FORMAT_HEXADECIMAL_HH

# include <string>

# include <elle/utility/Buffer.hh>

namespace elle
{
  namespace format
  {
    namespace hexadecimal
    {

      /// Convert any binary data to an hexadecimal encoded string.
      std::string
      encode(char const* binary_data,
             size_t size);

      /// Convert any binary data to an hexadecimal encoded string.
      /// This version append the result to the given string.
      void
      encode(char const* binary_data,
             size_t size,
             std::string& hexadecimal_string);

      /// Convert any hexadecimal to a decoded binary data.
      void
      decode(std::string const& hexadecimal_string,
             elle::utility::Buffer& binary_data);

    }
  }
}

#endif
