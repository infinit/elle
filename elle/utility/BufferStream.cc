
#include <elle/utility/Buffer.hh>
#include <elle/utility/BufferStream.hh>

using namespace elle::utility;

InputBufferStream::InputBufferStream(WeakBuffer const& input)
  : _buffer(input)
  , _idx(0)
{}

InputBufferStream::InputBufferStream(InputBufferStream const& other)
  : _buffer(other._buffer)
  , _idx(other._idx)
{}

size_t InputBufferStream::BytesLeft() const
{
  return this->_buffer.Size() - this->_idx;
}

void InputBufferStream::read(char* out, std::streamsize size)
{
  if (this->_idx + size > this->_buffer.Size())
    throw std::runtime_error("Out of range");
  std::uninitialized_copy(
      this->_buffer.Contents() + this->_idx,
      this->_buffer.Contents() + this->_idx + size,
      out
  );
}


OutputBufferStream::OutputBufferStream(Buffer& output)
  : _buffer(output)
{}

OutputBufferStream::OutputBufferStream(OutputBufferStream const& other)
  : _buffer(other._buffer)
{}

void OutputBufferStream::write(char const* in, std::streamsize size)
{
  this->_buffer.Append(in, size);
}
