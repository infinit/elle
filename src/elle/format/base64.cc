#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#include <elle/Exception.hh>
#include <elle/assert.hh>
#include <elle/format/base64.hh>
#include <elle/log.hh>

ELLE_LOG_COMPONENT("elle.format.base64")

namespace elle
{
  namespace format
  {
    namespace base64
    {
      using namespace boost::archive::iterators;

      size_t
      encoded_size(ConstWeakBuffer input)
      {
        return (signed(input.size()) + 2) / 3 * 4;
      }

      Buffer
      encode(ConstWeakBuffer input)
      {
        ELLE_TRACE_SCOPE("encode %s", input);
        size_t size = encoded_size(input);
        ELLE_DEBUG("previsional size: %s", size);
        Buffer res;
        res.capacity(size);
        IOStream stream(res.ostreambuf());
        Stream base64_stream(stream);
        base64_stream.write(reinterpret_cast<char const*>(input.contents()),
                            input.size());
        return res;
      }

      size_t
      decoded_size(ConstWeakBuffer encoded)
      {
        size_t size = encoded.size();
        int padding = 0;
        if (size >= 1 && encoded.contents()[size - 1] == '=')
          ++padding;
        if (size >= 2 && encoded.contents()[size - 2] == '=')
          ++padding;
        return size / 4 * 3 - padding;
      }

      Buffer
      decode(ConstWeakBuffer input)
      {
        ELLE_TRACE_SCOPE("decode %s", input);
        size_t size = decoded_size(input);
        ELLE_DEBUG("previsional size: %s", size);
        Buffer res(size);
        IOStream stream(input.istreambuf());
        Stream base64_stream(stream);
        base64_stream.read(reinterpret_cast<char*>(res.mutable_contents()),
                           size);
        ELLE_ASSERT_EQ(base64_stream.gcount(), signed(size));
        ELLE_ASSERT(
          (base64_stream.read(reinterpret_cast<char*>(
                                res.mutable_contents()), 1),
           base64_stream.gcount() == 0));

        return res;
      }

      using encoder = base64_from_binary<transform_width<const char *, 6, 8>>;
      template <typename SourceIterator, typename DestinationIterator>
      void
      _stream_encode(SourceIterator begin,
                     SourceIterator end,
                     DestinationIterator output)
      {
        std::copy(encoder(begin), encoder(end), output);
      }

      using decoder = transform_width<binary_from_base64<char const *>, 8, 6>;
      template <typename SourceIterator, typename DestinationIterator>
      void
      _stream_decode(SourceIterator begin,
                     SourceIterator end,
                     DestinationIterator output)
      {
        std::copy(decoder(begin), decoder(end), output);
      }

      /*-------------.
      | Construction |
      `-------------*/

      StreamBuffer::StreamBuffer(std::iostream& stream):
        _stream(stream),
        _remaining_write(0),
        _remaining_read(0)
      {}

      /*-------.
      | Buffer |
      `-------*/

      WeakBuffer
      StreamBuffer::read_buffer()
      {
        static auto const size = sizeof(this->_buffer_read) / 3 * 4;
        ELLE_TRACE_SCOPE("%s: decode up to %s bytes", *this, size);
        char buffer[size];
        size_t read;
        ELLE_DEBUG("%s: read up to %s bytes from the backend", *this, size)
        {
          this->_stream.read(buffer, size);
          read = this->_stream.gcount();
          ELLE_DUMP("%s: got %s bytes", *this, read);
          if (read > 0)
            ELLE_DUMP("%s: encoded data: %s", *this,
                      elle::WeakBuffer(buffer, read));
        }
        this->_remaining_read = read % 4;
        read = read - this->_remaining_read;
        size_t decoded_size = read / 4 * 3;
        if (read > 0)
        {
          ELLE_DEBUG_SCOPE("%s: decode %s bytes", *this, read);
          bool end = buffer[read - 1] == '=';
          if (end)
          {
            read -= 4;
            decoded_size -= 3;
          }
          _stream_decode(buffer, buffer + read, this->_buffer_read);
          if (end)
          {
            ELLE_DEBUG_SCOPE("%s: decode last 4 bytes with padding", *this);
            ELLE_DUMP("%s: remaining bytes: \"%s\"", *this,
                      std::string(buffer + read, 4));
            auto source = decoder(buffer + read);
            auto destination = this->_buffer_read + (read / 4 * 3);
            if (buffer[read + 2] != '=')
            {
              *(destination++) = *(source++);
              ++decoded_size;
            }
            *(destination++) = *(source++);
            ++decoded_size;
          }
          if (decoded_size > 0)
            ELLE_DUMP("%s: decoded data: %s", *this,
                      elle::WeakBuffer(this->_buffer_read, decoded_size));
        }
        if (this->_remaining_read > 0)
        {
          ELLE_DEBUG_SCOPE("%s: store %s remaining bytes",
                           *this, this->_remaining_read);
        }
        return WeakBuffer(this->_buffer_read, decoded_size);
      }

      WeakBuffer
      StreamBuffer::write_buffer()
      {
        return WeakBuffer(this->_buffer_write + this->_remaining_write,
                          sizeof(_buffer_write) - this->_remaining_write);
      }

      void
      StreamBuffer::flush(Size size)
      {
        size += this->_remaining_write;
        ELLE_TRACE_SCOPE("%s: encode %s bytes", *this, size);
        this->_remaining_write = size % 3;
        size = size - size % 3;
        if (size > 0)
        {
          ELLE_DEBUG_SCOPE("%s: encode %s bytes to the backend",
                           *this, size);
          _stream_encode(this->_buffer_write,
                         this->_buffer_write + size,
                         std::ostreambuf_iterator<char>(_stream));
        }
        if (size && this->_remaining_write > 0)
        {
          ELLE_DEBUG_SCOPE("%s: push back %s bytes in the buffer",
                           *this, this->_remaining_write);
          memcpy(this->_buffer_write,
                 this->_buffer_write + size,
                 this->_remaining_write);
        }
      }

      void
      StreamBuffer::finalize()
      {
        if (this->_remaining_write > 0)
        {
          ELLE_DEBUG_SCOPE("%s: encode last %s remaining bytes",
                           *this, this->_remaining_write);
          ELLE_ASSERT_LT(this->_remaining_write, 3);
          _stream_encode(
            this->_buffer_write,
            this->_buffer_write + this->_remaining_write,
            std::ostreambuf_iterator<char>(this->_stream));
          switch (this->_remaining_write)
          {
            case 1:
              this->_stream << "==";
              break;
            case 2:
              this->_stream << "=";
              break;
            default:
              elle::unreachable();
          }
        }
      }

      /*----------.
      | Printable |
      `----------*/

      void
      StreamBuffer::print(std::ostream& output) const
      {
        output << "base64::StreamBuffer";
      }

      Stream::Stream(std::iostream& underlying):
        IOStream(this->_buffer = new StreamBuffer(underlying)),
        _underlying(underlying)
      {}

      Stream::~Stream()
      {
        ELLE_TRACE_SCOPE("%s: finalize encoding", *this->_buffer);
        this->_buffer->sync();
        this->_buffer->finalize();
      }
    }
  }
}
