#include <zlib.h>

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
      static
      int
      z_call(z_stream* stream, int err)
      {
        ELLE_ASSERT_NEQ(err, Z_STREAM_ERROR);
        if (err == Z_MEM_ERROR)
          throw std::bad_alloc();
        else if (err != Z_STREAM_END && err != Z_OK)
          elle::err("ZLIB error: {}: {}", zError(err), stream->msg);
        return err;
      }

      class StreamBuffer
        : public elle::StreamBuffer
      {
      public:
        StreamBuffer(std::iostream& underlying,
                     bool honor_flush,
                     Buffer::Size buffer_size)
          : _underlying(underlying)
          , _honor_flush(honor_flush)
          , _buffer_size(buffer_size)
          , _deflate_stream()
          , _deflate_buffer(0)
          , _deflate_z_buffer(0)
          , _deflate_init(false)
          , _inflate_stream()
          , _inflate_buffer(0)
          , _inflate_z_buffer(0)
          , _inflate_init(false)
        {}

        ~StreamBuffer()
        {
          if (this->_deflate_init)
          {
            ELLE_TRACE_SCOPE("%s: finalize output", this);
            this->_deflate_stream.avail_in = 0;
            this->_deflate_stream.next_in = this->_deflate_buffer.contents();
            this->_send(Z_FINISH);
            deflateEnd(&this->_deflate_stream);
          }
          if (this->_inflate_init)
            inflateEnd(&this->_inflate_stream);
        }

        WeakBuffer
        write_buffer() override
        {
          if (!this->_deflate_init)
          {
            this->_deflate_init = true;
            this->_deflate_buffer.size(this->_buffer_size);
            this->_deflate_stream.zalloc = Z_NULL;
            this->_deflate_stream.zfree = Z_NULL;
            this->_deflate_stream.opaque = Z_NULL;
            this->_deflate_z_buffer.capacity(this->_buffer_size);
            z_call(
              &this->_deflate_stream,
              deflateInit2(
                &this->_deflate_stream,
                // Compression level.
                Z_DEFAULT_COMPRESSION,
                // Compression algorithm.
                Z_DEFLATED,
                // Window size: 15 is "big", 16 means "do gzip, not zlib".
                15 + 16,
                // Internal buffer.
                8,
                // Default strategy: no assumption on the nature of the data.
                Z_DEFAULT_STRATEGY));
          }
          return WeakBuffer(this->_deflate_buffer);
        }

        void
        _send(int flush)
        {
          // Deflate until it fails to fill the output buffer.
          do
          {
            auto& output = this->_deflate_z_buffer;
            this->_deflate_stream.avail_out = output.capacity();
            this->_deflate_stream.next_out = output.contents();
            auto ret = z_call(&this->_deflate_stream,
                              deflate(&this->_deflate_stream, flush));
            ELLE_ASSERT_NEQ(ret, Z_STREAM_ERROR);
            ELLE_ASSERT_NEQ(ret, Z_BUF_ERROR);
            output.size(output.capacity() - this->_deflate_stream.avail_out);
            ELLE_DEBUG("%s: send %s compressed bytes to underlying stream",
                       *this, output.size())
            {
              ELLE_DUMP("%s: compressed data: %s", *this, output);
              this->_underlying.write(
                reinterpret_cast<char const*>(output.contents()), output.size());
            }
          }
          while (this->_deflate_stream.avail_out == 0);
          ELLE_ASSERT(this->_deflate_stream.avail_in == 0);
        }

        ConstWeakBuffer
        read_buffer() override
        {
          if (!this->_inflate_init)
          {
            ELLE_DEBUG("{}: initialize decompression", this);
            this->_inflate_init = true;
            this->_inflate_stream.zalloc = Z_NULL;
            this->_inflate_stream.zfree = Z_NULL;
            this->_inflate_stream.opaque = Z_NULL;
            this->_inflate_stream.avail_in = 0;
            this->_inflate_stream.next_in = Z_NULL;
            this->_inflate_z_buffer.capacity(this->_buffer_size);
            this->_inflate_buffer.capacity(this->_buffer_size);
            z_call(&this->_inflate_stream,
                   inflateInit2(&this->_inflate_stream, 15 + 32));
          }
          this->_inflate_stream.next_out =
            this->_inflate_buffer.mutable_contents();
          this->_inflate_stream.avail_out =
            this->_inflate_buffer.capacity();
          while (this->_inflate_stream.avail_out > 0)
          {
            if (this->_inflate_stream.avail_in == 0)
            {
              ELLE_DEBUG_SCOPE("{}: read compressed bytes", this);
              this->_inflate_stream.next_in =
                this->_inflate_z_buffer.contents();
              // FIXME: check bad
              this->_inflate_stream.avail_in =
                this->_underlying.readsome(
                  reinterpret_cast<char*>(this->_inflate_z_buffer.contents()),
                  this->_inflate_z_buffer.capacity());
              if (this->_inflate_stream.avail_in == 0)
                return {};
              this->_inflate_z_buffer.size(this->_inflate_stream.avail_in);
              ELLE_DEBUG("{}: got {} compressed bytes",
                         this, this->_inflate_stream.avail_in);
              ELLE_DUMP("{}: compressed bytes: {}",
                        this, this->_inflate_z_buffer);
            }
            {
              ELLE_DEBUG_SCOPE("{}: decompress from {} available bytes",
                               this, this->_inflate_stream.avail_in);
              auto before = this->_inflate_stream.avail_in;
              auto ret = z_call(&this->_inflate_stream,
                                inflate(&this->_inflate_stream, Z_NO_FLUSH));
              this->_inflate_buffer.size(
                this->_inflate_buffer.capacity() -
                this->_inflate_stream.avail_out);
              auto after = this->_inflate_stream.avail_in;
              ELLE_DEBUG("{}: decompressed {} bytes from {} bytes",
                         this, this->_inflate_buffer.size(), before - after);
              ELLE_DUMP("{}: decompressed bytes: {}",
                        this, this->_inflate_buffer);
              if (ret == Z_STREAM_END)
                break;
              ELLE_ASSERT(before > after || this->_inflate_buffer.size() > 0);
            }
          }
          return WeakBuffer(this->_inflate_buffer);
        }

        void
        flush(Size size) override
        {
          if (size == 0)
            return;
          ELLE_TRACE_SCOPE("%s: compress %s bytes", *this, size);
          ELLE_ASSERT_LTE(size, this->_deflate_buffer.size());
          // Define input.
          elle::ConstWeakBuffer input(this->_deflate_buffer.contents(), size);
          ELLE_DUMP("%s: data: %s", input, input);
          this->_deflate_stream.avail_in = input.size();
          this->_deflate_stream.next_in = const_cast<unsigned char*>(input.contents());
          _send(this->_honor_flush ? Z_SYNC_FLUSH : Z_NO_FLUSH);
          this->_underlying.flush();
        }

      private:
        ELLE_ATTRIBUTE(std::iostream&, underlying);
        ELLE_ATTRIBUTE(bool, honor_flush);
        ELLE_ATTRIBUTE(Buffer::Size, buffer_size);
        ELLE_ATTRIBUTE(z_stream, deflate_stream);
        ELLE_ATTRIBUTE(elle::Buffer, deflate_buffer);
        ELLE_ATTRIBUTE(elle::Buffer, deflate_z_buffer);
        ELLE_ATTRIBUTE(bool, deflate_init);
        ELLE_ATTRIBUTE(z_stream, inflate_stream);
        ELLE_ATTRIBUTE(elle::Buffer, inflate_buffer);
        ELLE_ATTRIBUTE(elle::Buffer, inflate_z_buffer);
        ELLE_ATTRIBUTE(bool, inflate_init);
      };

      Stream::Stream(std::iostream& underlying,
                     bool honor_flush,
                     Buffer::Size buffer_size):
        IOStream(new StreamBuffer(underlying, honor_flush, buffer_size))
      {}
    }
  }
}
