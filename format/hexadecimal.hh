#ifndef  ELLE_FORMAT_HEXADECIMAL_HH
# define ELLE_FORMAT_HEXADECIMAL_HH

# include <string>

# include <elle/Buffer.hh>

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
      /// This version appends the result to the given string.
      void
      encode(char const* binary_data,
             size_t size,
             std::string& hexadecimal_string);
      /// Convert any hexadecimal string to a buffer.
      elle::Buffer
      decode(std::string const& hexadecimal_string);
      /// Convert any hexadecimal to a decoded binary data.
      /// This version appends the result to the given buffer.
      void
      decode(std::string const& hexadecimal_string,
             elle::Buffer& binary_data);
    }
  }
}

#endif
