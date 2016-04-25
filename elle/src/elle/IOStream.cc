#include <elle/Buffer.hh>
#include <elle/IOStream.hh>
#include <elle/log.hh>

#include <streambuf>

#include <sys/types.h>
#include <unistd.h>

ELLE_LOG_COMPONENT("elle.IOStream");

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

  IOStream::IOStream(std::streambuf* buffer):
    std::iostream(buffer),
    _buffer(buffer)
  {
    exceptions(std::iostream::badbit);
  }

  IOStream::IOStream(IOStream&& source):
    std::iostream(source._buffer),
    _buffer(source._buffer)
  {
    source._buffer = nullptr;
  }

  IOStream::~IOStream()
  {
    if (this->_buffer)
    {
      this->_buffer->pubsync();
      delete _buffer;
    }
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
    Size size = pptr() - pbase();
    ELLE_TRACE_SCOPE("%s: sync %s bytes", *this, size);
    setp(0, 0);
    if (size > 0)
      flush(size);
    // Success
    return 0;
  }

  void
  StreamBuffer::flush(Size)
  {}

  /*------------------.
  | PlainStreamBuffer |
  `------------------*/

  PlainStreamBuffer::PlainStreamBuffer():
    _ibuf(),
    _obuf()
  {}

  PlainStreamBuffer::~PlainStreamBuffer()
  {}

  WeakBuffer
  PlainStreamBuffer::read_buffer()
  {
    static Size max_size = _bufsize;
    ELLE_TRACE("read at most %s bytes", max_size)
    {
      Size size = read(_ibuf, max_size);
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
  PlainStreamBuffer::flush(Size size)
  {
    ELLE_TRACE("write %s bytes", size)
      write(_obuf, size);
  }

  /*-------------------.
  | DynamicStreamBuffer |
  `-------------------*/

  DynamicStreamBuffer::DynamicStreamBuffer(Size size):
    _bufsize(size),
    _ibuf(new Byte[size]),
    _obuf(new Byte[size])
  {}

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
      Size size = read((char *)_ibuf, this->_bufsize);
      ELLE_TRACE("got %s bytes", size);
      return WeakBuffer{this->_ibuf, size};
    }
  }

  WeakBuffer
  DynamicStreamBuffer::write_buffer()
  {
    return WeakBuffer{this->_obuf, _bufsize};
  }

  void
  DynamicStreamBuffer::flush(Size size)
  {
    ELLE_TRACE("write %s bytes", size)
      write((char *)this->_obuf, size);
  }
}

namespace std
{
  // Read up to \a n bytes, but at least one, unlike std::istream::readsome.
  streamsize
  readsome(std::istream& i, char* s, streamsize n)
  {
    if (n == 0)
      return 0;
    auto res = i.readsome(s, n);
    if (res > 0)
      return res;
    if (i.eof())
      return 0;
    int first = i.get();
    if (first == EOF)
      return 0;
    s[0] = first;
    res = 1;
    if (n >= 1)
      res += i.readsome(s + 1, n - 1);
    return res;
  }
}
