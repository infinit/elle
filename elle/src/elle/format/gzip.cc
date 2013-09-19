#include <elle/format/gzip.hh>

namespace elle
{
  namespace format
  {
    namespace gzip
    {
      static Size const buffer_size = 1 << 16;

      StreamBuffer::StreamBuffer(std::ostream& underlying):
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
        if (size > buffer_size)
          throw elle::Exception("Not enough space in the Gzip buffer.");

        boost::iostreams::filtering_ostream filter;
        filter.push(boost::iostreams::gzip_compressor());
        filter.push(_underlying);
        filter.write(reinterpret_cast<char*>(_buffer.mutable_contents()),
                     size);
      }

      Stream::Stream(std::ostream& underlying):
        IOStream(new StreamBuffer(underlying)),
        _underlying(underlying)
      {}
    }
  }
}
