#pragma once

#include <iostream>

#include <elle/compiler.hh>
#include <elle/fwd.hh>

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
    ~IOStream() override;

  protected:
    std::streambuf* _buffer;
  };

  class ELLE_API StreamBuffer
    : public std::streambuf
  {
  public:
    using Size = uint64_t;
    StreamBuffer();

    ~StreamBuffer() override;

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

    int
    underflow() override;


    int
    overflow(int c) override;


    int
    sync() override;
  };

  /// Simple implementation of a streambuf with local buffers.
  class ELLE_API PlainStreamBuffer
    : public StreamBuffer
  {
  public:
    using Size = StreamBuffer::Size;
    PlainStreamBuffer();
    ~PlainStreamBuffer() override;

  protected:
    friend class IOStream;
    virtual Size read (char* buffer, Size size) = 0;
    virtual void write(char* buffer, Size size) = 0;


    WeakBuffer
    write_buffer() override;


    WeakBuffer
    read_buffer() override;


    void
    flush(Size size) override;

  private:
    static const int _bufsize = (1 << 12); // == 4096 bytes.
    char _ibuf[_bufsize];
    char _obuf[_bufsize];
  };

  class ELLE_API DynamicStreamBuffer
    : public StreamBuffer
  {
  public:
    using Size = StreamBuffer::Size;
    using Byte = unsigned char;

    DynamicStreamBuffer(Size size);
    ~DynamicStreamBuffer() override;

  protected:
    friend class IOStream;
    virtual Size read(char *buffer, Size size) = 0;
    virtual void write(char *buffer, Size size) = 0;


    WeakBuffer
    read_buffer() override;


    WeakBuffer
    write_buffer() override;


    void
    flush(Size size) override;

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
