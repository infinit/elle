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
      class StreamBuffer;

      class Stream:
        public elle::IOStream
      {
      public:
        Stream(std::iostream& underlying);
        ~Stream();

      private:
        friend class StreamBuffer;
        ELLE_ATTRIBUTE(StreamBuffer*, buffer);
        ELLE_ATTRIBUTE_R(std::iostream&, underlying);
      };

      /// Encode to base64.
      Buffer
      encode(WeakBuffer clear);

      /// Decode from base64.
      Buffer
      decode(WeakBuffer input);
    }
  }
}

#endif
