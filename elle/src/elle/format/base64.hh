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
      /// Encode a buffer into a base64 string.
      /// @note `clear` can be a `Buffer` or a `WeakBuffer` instance.
      template <typename T>
      std::string
      encode(T&& clear);

      /// Encode a buffer and append it to the base64 string.
      /// @note `clear` can be a `Buffer` or a `WeakBuffer` instance.
      template <typename T>
      void
      encode(T&& clear,
             std::string& base64);

      /// Decode a base64 string into a buffer.
      Buffer
      decode(std::string const& base64);

      /// Decode a base64 string and append it to the clear buffer.
      void
      decode(std::string const& base64,
             Buffer& clear);
    }
  }
}

# include "base64.hxx"

#endif
