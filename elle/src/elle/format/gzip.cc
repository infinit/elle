#include <elle/format/gzip.hh>

namespace elle
{
  namespace format
  {
    namespace gzip
    {
      StreamBuffer::StreamBuffer(std::ostream& underlying,
                                 Buffer::Size buffer_size):
        _underlying(underlying),
        _buffer(buffer_size)
      {}

      WeakBuffer
      StreamBuffer::write_buffer()
      {
        return WeakBuffer(_buffer);
      }

      WeakBuffer
      StreamBuffer::read_buffer()
      {
        throw elle::Exception("Gzip decompression not handled yet");
      }

      void
      StreamBuffer::flush(Size size)
      {
        ELLE_ASSERT_LTE(size, this->_buffer.size());

        boost::iostreams::filtering_ostream filter;
        filter.push(boost::iostreams::gzip_compressor());
        filter.push(_underlying);
        filter.write(reinterpret_cast<char*>(_buffer.mutable_contents()),
                     size);
      }

      Stream::Stream(std::ostream& underlying, Buffer::Size buffer_size):
        IOStream(new StreamBuffer(underlying, buffer_size)),
        _underlying(underlying)
      {}
    }
  }
}
