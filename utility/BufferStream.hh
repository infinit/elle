#ifndef  ELLE_UTILITY_BUFFERSTREAM_HH
# define ELLE_UTILITY_BUFFERSTREAM_HH

# include <elle/utility/fwd.hh>

namespace elle
{
  namespace utility
  {

    class InputBufferStream
    {
    private:
      Buffer const& _buffer;
      size_t        _idx;

    public:
      InputBufferStream(Buffer const& input);
      InputBufferStream(InputBufferStream const& other);

      size_t BytesLeft() const  { return this->_buffer.Size() - this->_idx; }
      size_t Offset() const     { return this->_idx; }

    public:
    /// The following methods makes this class "std::istream like".
      void read(char* out, std::streamsize size);

    };

    class OutputBufferStream
    {
    private:
      Buffer&       _buffer;

    public:
      OutputBufferStream(Buffer& output);
      OutputBufferStream(OutputBufferStream& other);
    };

  }
}

#endif

