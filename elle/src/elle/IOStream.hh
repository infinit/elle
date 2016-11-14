#ifndef ELLE_IO_STREAM_HH
# define ELLE_IO_STREAM_HH

# include <iostream>

# include <elle/fwd.hh>

namespace elle
{
  class StreamBuffer;

  /// RAII helper to call clear on std streams.
  class ELLE_API IOStreamClear
  {
  public:
    IOStreamClear (std::ios& s);
    ~IOStreamClear();

  private:
    std::ios& _stream;
  };

  class ELLE_API IOStream
    : public std::iostream
  {
  public:
    IOStream(std::streambuf* buffer);
    IOStream(IOStream&& buffer);
    ~IOStream();

  protected:
    std::streambuf* _buffer;
  };

  class ELLE_API StreamBuffer
    : public std::streambuf
  {
  public:
    typedef uint64_t Size;
    StreamBuffer();
    virtual
    ~StreamBuffer();

  /// API to override.
  protected:
    friend class IOStream;

    /// The buffer where to write.
    virtual
    WeakBuffer
    write_buffer() = 0;

    /// The buffer with the next available data.
    virtual
    WeakBuffer
    read_buffer()  = 0;

    /// Synchronize the write buffer to the underlying implementation.
    virtual
    void
    flush(Size size);

  /// std::streambuf interface.
  protected:
    virtual
    int
    underflow();

    virtual
    int
    overflow(int c);

    virtual
    int
    sync();
  };

  /// Simple implementation of a streambuf with local buffers.
  class ELLE_API PlainStreamBuffer
    : public StreamBuffer
  {
  public:
    typedef StreamBuffer::Size Size;
    PlainStreamBuffer();
    ~PlainStreamBuffer();

  protected:
    friend class IOStream;
    virtual Size read (char* buffer, Size size) = 0;
    virtual void write(char* buffer, Size size) = 0;

    virtual
    WeakBuffer
    write_buffer();

    virtual
    WeakBuffer
    read_buffer();

    virtual
    void
    flush(Size size);

  private:
    static const int _bufsize = (1 << 12); // == 4096 bytes.
    char _ibuf[_bufsize];
    char _obuf[_bufsize];
  };

  class ELLE_API DynamicStreamBuffer
    : public StreamBuffer
  {
  public:
    typedef StreamBuffer::Size Size;
    typedef unsigned char Byte;

    DynamicStreamBuffer(Size size);
    ~DynamicStreamBuffer();

  protected:
    friend class IOStream;
    virtual Size read(char *buffer, Size size) = 0;
    virtual void write(char *buffer, Size size) = 0;

    virtual
    WeakBuffer
    read_buffer();

    virtual
    WeakBuffer
    write_buffer();

    virtual
    void
    flush(Size size);

  private:
    Size const _bufsize;
    Byte *_ibuf;
    Byte *_obuf;
  };
}

namespace std
{
  // Read up to \a n bytes, but at least one, unlike std::istream::readsome.
  ELLE_API
  streamsize
  readsome(std::istream& i, char* s, streamsize n);
}

#endif
