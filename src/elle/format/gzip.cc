# include <zlib.h>

#include <elle/finally.hh>
#include <elle/format/gzip.hh>
#include <elle/log.hh>

// Do not think about using Boost IOstream for GZip compression. It has a nasty
// bug with empty gzip streams (see
// https://svn.boost.org/trac/boost/ticket/5237) and doesn't handle flushing.

ELLE_LOG_COMPONENT("elle.format.gzip");

namespace elle
{
  namespace format
  {
    namespace gzip
    {
      class StreamBuffer:
        public elle::StreamBuffer
      {
      public:
        StreamBuffer(std::ostream& underlying,
                     bool honor_flush,
                     Buffer::Size buffer_size);
        ~StreamBuffer();

        virtual WeakBuffer write_buffer();
        virtual WeakBuffer read_buffer();
        virtual void flush(Size size);

      private:
        void
        _send(int flush);
        ELLE_ATTRIBUTE(bool, honor_flush);
        ELLE_ATTRIBUTE(z_stream, z_stream);
        ELLE_ATTRIBUTE(std::ostream&, underlying);
        ELLE_ATTRIBUTE(elle::Buffer, buffer);
        ELLE_ATTRIBUTE(elle::Buffer, buffer_compression);
      };

      StreamBuffer::StreamBuffer(std::ostream& underlying,
                                 bool honor_flush,
                                 Buffer::Size buffer_size):
        _honor_flush(honor_flush),
        _z_stream(),
        _underlying(underlying),
        _buffer(buffer_size),
        _buffer_compression(0)
      {
        this->_buffer_compression.capacity(buffer_size);
        this->_z_stream.zalloc = Z_NULL;
        this->_z_stream.zfree = Z_NULL;
        this->_z_stream.opaque = Z_NULL;
        {
          auto err = deflateInit2(
            &this->_z_stream,
            // Compression level.
            Z_DEFAULT_COMPRESSION,
            // Compression algorithm.
            Z_DEFLATED,
            // Window size: 15 is "big", 16 means "do gzip, not zlib".
            15 + 16,
            // Internal buffer.
            8,
            // Default strategy: no assumption on the nature of the data.
            Z_DEFAULT_STRATEGY);
          if (err == Z_MEM_ERROR)
            throw std::bad_alloc();
          else if (err != Z_OK)
            throw elle::Exception(
              elle::sprintf("ZLIB deflateInit error: %s", err));
        }
      }

      StreamBuffer::~StreamBuffer()
      {
        ELLE_TRACE_SCOPE("%s: flush remaining content", *this);
        this->_z_stream.avail_in = 0;
        this->_z_stream.next_in = this->_buffer.contents();
        _send(Z_FINISH);
        deflateEnd(&this->_z_stream);
      }

      void
      StreamBuffer::_send(int flush)
      {
        // Deflate until it fails to fill the output buffer.
        do
        {
          auto& output = this->_buffer_compression;
          this->_z_stream.avail_out = output.capacity();
          this->_z_stream.next_out = output.contents();
          auto ret = deflate(&this->_z_stream, flush);
          ELLE_ASSERT_NEQ(ret, Z_STREAM_ERROR);
          ELLE_ASSERT_NEQ(ret, Z_BUF_ERROR);
          output.size(output.capacity() - this->_z_stream.avail_out);
          ELLE_DEBUG("%s: send %s compressed bytes to underlying stream",
                     *this, output.size())
          {
            ELLE_DUMP("%s: compressed data: %s", *this, output);
            this->_underlying.write(
              reinterpret_cast<char const*>(output.contents()), output.size());
          }
        }
        while (this->_z_stream.avail_out == 0);
        ELLE_ASSERT(this->_z_stream.avail_in == 0);
      }

      WeakBuffer
      StreamBuffer::write_buffer()
      {
        return WeakBuffer(this->_buffer);
      }

      WeakBuffer
      StreamBuffer::read_buffer()
      {
        throw elle::Exception("Gzip decompression not handled yet");
      }

      void
      StreamBuffer::flush(Size size)
      {
        if (size == 0)
          return;

        ELLE_TRACE_SCOPE("%s: compress %s bytes", *this, size);
        ELLE_ASSERT_LTE(size, this->_buffer.size());

        // Define input.
        elle::ConstWeakBuffer input(this->_buffer.contents(), size);
        ELLE_DUMP("%s: data: %s", input, input);
        this->_z_stream.avail_in = input.size();
        this->_z_stream.next_in = const_cast<unsigned char*>(input.contents());

        _send(this->_honor_flush ? Z_SYNC_FLUSH : Z_NO_FLUSH);
        this->_underlying.flush();
      }

      Stream::Stream(std::ostream& underlying,
                     bool honor_flush,
                     Buffer::Size buffer_size):
        IOStream(new StreamBuffer(underlying, honor_flush, buffer_size))
      {}
    }
  }
}
