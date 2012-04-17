
#include <elle/utility/Buffer.hh>
#include <elle/utility/BufferStream.hh>

using namespace elle::utility;

InputBufferStream::InputBufferStream(Buffer const& input)
  : _buffer(input)
{}

OutputBufferStream::OutputBufferStream(Buffer& output)
  : _buffer(output)
{}
