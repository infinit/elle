#include <streambuf>
#include <unistd.h>

#include <elle/log.hh>
#include <elle/IOStream.hh>

ELLE_LOG_TRACE_COMPONENT("Elle.IOStream");

namespace elle
{
  /*---------.
  | IOStream |
  `---------*/

  IOStream::IOStream(StreamBuffer* buffer)
    : std::iostream(_buffer = buffer)
  {
    exceptions(std::iostream::badbit);
  }

  IOStream::~IOStream()
  {
    _buffer->sync();
    delete _buffer;
  }

  /*-------------.
  | StreamBuffer |
  `-------------*/

  StreamBuffer::StreamBuffer()
  {
    setg(0, 0, 0);
    setp(0, 0);
  }

  StreamBuffer::~StreamBuffer()
  {}

  int
  StreamBuffer::underflow()
  {
    Buffer b = read_buffer();
    if (b.size == 0)
      {
        setg(0, 0, 0);
        return EOF;
      }
    setg(b.data, b.data, &b.data[b.size]);
    return static_cast<unsigned char>(b.data[0]);
  }

  int
  StreamBuffer::overflow(int c)
  {
    sync();
    Buffer b = write_buffer();
    setp(b.data, &b.data[b.size]);
    b.data[0] = static_cast<char>(c);
    pbump(1);
    // Success is indicated by "A value different from EOF".
    return EOF + 1;
  }

  int
  StreamBuffer::sync()
  {
    unsigned int size = pptr() - pbase();
    if (size > 0)
      flush(size);
    setp(0, 0);
    // Success
    return 0;
  }

  void
  StreamBuffer::flush(unsigned int size)
  {}

  /*------------------.
  | PlainStreamBuffer |
  `------------------*/

  PlainStreamBuffer::PlainStreamBuffer()
    : _ibuf()
    , _obuf()
  {}

  PlainStreamBuffer::~PlainStreamBuffer()
  {}

  Buffer
  PlainStreamBuffer::read_buffer()
  {
    static const int max_size = _bufsize;
    ELLE_LOG_TRACE("read at most %s bytes", max_size)
      {
        ssize_t size = read(_ibuf, max_size);
        ELLE_LOG_TRACE("got %s bytes", size);
        return Buffer(_ibuf, size);
      }
  }

  Buffer
  PlainStreamBuffer::write_buffer()
  {
    return Buffer(_obuf, _bufsize);
  }

  void
  PlainStreamBuffer::flush(unsigned int size)
  {
    ELLE_LOG_TRACE("write %s bytes", size)
      write(_obuf, size);
  }
}
