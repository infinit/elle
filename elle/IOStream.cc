#include <streambuf>
#include <unistd.h>

#include <elle/log.hh>
#include <elle/IOStream.hh>

ELLE_LOG_COMPONENT("Elle.IOStream");

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
    WeakBuffer b = read_buffer();
    if (b.size() == 0)
      {
        setg(0, 0, 0);
        return EOF;
      }
    setg(reinterpret_cast<char*>(b.mutable_contents()),
         reinterpret_cast<char*>(b.mutable_contents()),
         reinterpret_cast<char*>(b.mutable_contents() + b.size()));
    return static_cast<unsigned char>(b.contents()[0]);
  }

  int
  StreamBuffer::overflow(int c)
  {
    sync();
    WeakBuffer b = write_buffer();
    setp(reinterpret_cast<char*>(b.mutable_contents()),
         reinterpret_cast<char*>(b.mutable_contents() + b.size()));
    b.mutable_contents()[0] = static_cast<Byte>(c);
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
  StreamBuffer::flush(unsigned int)
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

  WeakBuffer
  PlainStreamBuffer::read_buffer()
  {
    static const int max_size = _bufsize;
    ELLE_TRACE("read at most %s bytes", max_size)
      {
        ssize_t size = read(_ibuf, max_size);
        ELLE_TRACE("got %s bytes", size);
        return WeakBuffer(_ibuf, size);
      }
  }

  WeakBuffer
  PlainStreamBuffer::write_buffer()
  {
    return WeakBuffer(_obuf, _bufsize);
  }

  void
  PlainStreamBuffer::flush(unsigned int size)
  {
    ELLE_TRACE("write %s bytes", size)
      write(_obuf, size);
  }
}
