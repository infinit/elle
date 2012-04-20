
#include <elle/utility/Buffer.hh>
#include <elle/utility/BufferStream.hh>

using namespace elle::utility;

InputBufferStream::InputBufferStream(Buffer const& input)
  : _buffer(input)
  , _idx(0)
{}

InputBufferStream::InputBufferStream(WeakBuffer const& input)
  : _buffer(input)
  , _idx(0)
{}

InputBufferStream::InputBufferStream(InputBufferStream const& other)
  : _buffer(other._buffer)
  , _idx(other._idx)
{}

OutputBufferStream::OutputBufferStream(Buffer& output)
  : _buffer(output)
{}

OutputBufferStream::OutputBufferStream(OutputBufferStream& other)
  : _buffer(other._buffer)
{}

void InputBufferStream::read(char* out, std::streamsize size)
{
}

size_t InputBufferStream::BytesLeft() const
{
  return this->_buffer.Size() - this->_idx;
}
