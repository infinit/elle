#ifndef  ELLE_UTILITY_BUFFERSTREAM_HH
# define ELLE_UTILITY_BUFFERSTREAM_HH

namespace elle
{
  namespace utility
  {
    class Buffer;

    class InputBufferStream
    {
    private:
      Buffer const& _buffer;

    public:
      InputBufferStream(Buffer const& input);
    };

    class OutputBufferStream
    {
    private:
      Buffer& _buffer;

    public:
      OutputBufferStream(Buffer& output);
    };

  }
}

#endif

