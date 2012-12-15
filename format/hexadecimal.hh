#ifndef ELLE_FORMAT_HEXADECIMAL_HH
# define ELLE_FORMAT_HEXADECIMAL_HH

# include <elle/types.hh>
# include <elle/Buffer.hh>

namespace elle
{
  namespace format
  {
    namespace hexadecimal
    {
      /// Convert any binary-based buffer to an hexadecimal encoded string.
      template <typename T>
      String
      encode(T const& buffer);
      /// Convert any binary-based buffer to an hexadecimal encoded string.
      ///
      /// This version appends the result to the given string.
      template <typename T>
      void
      encode(T const& buffer,
             String& hexadecimal_string);
      /// Convert any hexadecimal string to a buffer.
      Buffer
      decode(String const& hexadecimal_string);
      /// Convert any hexadecimal to a decoded binary data.
      ///
      /// This version appends the result to the given buffer.
      void
      decode(String const& hexadecimal_string,
             Buffer& binary_data);
    }
  }
}

# include <elle/format/hexadecimal.hxx>

#endif
