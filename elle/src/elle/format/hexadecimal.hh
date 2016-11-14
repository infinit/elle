#ifndef ELLE_FORMAT_HEXADECIMAL_HH
# define ELLE_FORMAT_HEXADECIMAL_HH

# include <string>

# include <elle/Buffer.hh>

namespace elle
{
  namespace format
  {
    namespace hexadecimal
    {
      /// Convert any binary-based buffer to an hexadecimal encoded string.
      ELLE_API
      std::string
      encode(ConstWeakBuffer buffer);
      /// Convert any binary-based buffer to an hexadecimal encoded string.
      ///
      /// This version appends the result to the given string.
      ELLE_API
      void
      encode(ConstWeakBuffer buffer,
             String& hexadecimal_string);
      /// Convert any hexadecimal string to a buffer.
      ELLE_API
      Buffer
      decode(String const& hexadecimal_string);
      /// Convert any hexadecimal to a decoded binary data.
      ///
      /// This version appends the result to the given buffer.
      ELLE_API
      void
      decode(String const& hexadecimal_string,
             Buffer& binary_data);
    }
  }
}

#endif
