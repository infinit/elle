#ifndef ELLE_FORMAT_BASE64_HXX
# define ELLE_FORMAT_BASE64_HXX

# include "base64.hh"

# include <elle/types.hh>
# include <elle/Buffer.hh>

# include <string>

namespace elle
{
  namespace format
  {
    namespace base64
    {
      template <typename T>
      std::string
      encode(T&& buffer)
      {
        std::string res;
        encode(std::forward<T>(buffer), res);
        return res;
      }

      template <typename T>
      void
      encode(T&& buffer,
             std::string& out)
      {
        detail::encode(
          static_cast<unsigned char*>(buffer.contents()), buffer.size(), out);
      }
    }
  }
}

#endif
