#ifndef ELLE_FORMAT_BASE64_HH
# define ELLE_FORMAT_BASE64_HH

# include <elle/types.hh>
# include <elle/Buffer.hh>

namespace elle
{
  namespace format
  {
    namespace base64
    {
      /// Convert any binary-based buffer to an base64 encoded string.
      template <typename T>
      std::string
      encode(T&& buffer);

      /// Convert any binary-based buffer to an base64 encoded string.
      ///
      /// This version appends the result to the given string.
      template <typename T>
      void
      encode(T&& buffer,
             std::string& base64_string);

      /// Convert any base64 string to a buffer.
      Buffer
      decode(std::string const& base64_string);

      /// Convert any base64 to a decoded binary data.
      ///
      /// This version appends the result to the given buffer.
      void
      decode(std::string const& base64_string,
             Buffer& binary_data);

      namespace detail
      {
        void encode(unsigned char const* in,
                    size_t const size,
                    std::string& out);
        char
        decode_char(unsigned char c);
      }
    }
  }
}

# include "base64.hxx"

#endif
