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

      /// The size of the encoded input.
      size_t
      encoded_size(WeakBuffer input);

      /// Decode from base64.
      Buffer
      decode(WeakBuffer input);

      /// The size of the decoded input.
      size_t
      decoded_size(WeakBuffer input);
    }
  }
}

#endif
