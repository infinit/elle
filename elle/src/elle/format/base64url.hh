#ifndef ELLE_FORMAT_BASE64URL_HH
# define ELLE_FORMAT_BASE64URL_HH

# include <elle/Buffer.hh>
# include <elle/format/base64.hh>
# include <elle/types.hh>

namespace elle
{
  namespace format
  {
    namespace base64url
    {
      class StreamBuffer;

      class Stream:
        public elle::IOStream
      {
      public:
        Stream(std::iostream& underlying);
        ~Stream();

      private:
        friend class StreamBuffer;
        ELLE_ATTRIBUTE(base64::StreamBuffer*, buffer);
        ELLE_ATTRIBUTE(elle::StreamBuffer*, rewrite_buffer);
        ELLE_ATTRIBUTE(elle::IOStream, rewritter);
      };

      /// Encode to base64url.
      Buffer
      encode(ConstWeakBuffer input);

      /// Decode from base64url.
      Buffer
      decode(ConstWeakBuffer input);
    }
  }
}

#endif
