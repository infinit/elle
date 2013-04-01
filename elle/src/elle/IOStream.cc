#include <streambuf>
#include <unistd.h>

#include <elle/log.hh>
#include <elle/IOStream.hh>

ELLE_LOG_COMPONENT("Elle.IOStream");

namespace elle
{
  /*--------------.
  | IOStreamClear |
  `--------------*/

  IOStreamClear::IOStreamClear(std::ios& s):
      _stream(s)
  {}

  IOStreamClear::~IOStreamClear()
  {
    _stream.clear();
  }

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
    ELLE_TRACE_SCOPE("%s: underflow", *this);
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
    ELLE_TRACE_SCOPE("%s: overflow", *this);
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
    ELLE_TRACE_SCOPE("%s: sync", *this);
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

  /*-------------------.
  | DynamicStreamBuffer |
  `-------------------*/

  DynamicStreamBuffer::DynamicStreamBuffer(Size size)
      : _bufsize{size}
      , _ibuf(new Byte[size])
      , _obuf(new Byte[size])
  {
  }

  DynamicStreamBuffer::~DynamicStreamBuffer()
  {
      delete [] this->_ibuf;
      delete [] this->_obuf;
  }

  WeakBuffer
  DynamicStreamBuffer::read_buffer()
  {
      ELLE_TRACE("read at most %s bytes", this->_bufsize)
        {
            ssize_t size = read((char *)_ibuf, this->_bufsize);
            ELLE_TRACE("got %s bytes", size);
            return WeakBuffer{this->_ibuf, static_cast<size_t>(size)};
        }
  }

  WeakBuffer
  DynamicStreamBuffer::write_buffer()
  {
      ELLE_TRACE("return WeakBuffer{%p, %d}", this->_obuf, _bufsize);
      return WeakBuffer{this->_obuf, _bufsize};
  }

  void
  DynamicStreamBuffer::flush(unsigned int size)
  {
    ELLE_TRACE("write %s bytes", size)
      write((char *)this->_obuf, size);
  }
}
