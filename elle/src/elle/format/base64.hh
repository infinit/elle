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
      /// Encode to base64.
      Buffer
      encode(Buffer const& clear);

      /// Decode from base64.
      Buffer
      decode(Buffer const& input);
    }
  }
}

#endif
