#ifndef ELLE_IO_STREAM_HH
# define ELLE_IO_STREAM_HH

# include <iostream>

# include <elle/Buffer.hh>

namespace elle
{
  class StreamBuffer;

  class IOStream: public std::iostream
  {
  public:
    IOStream(StreamBuffer* buffer);
    ~IOStream();

  protected:
    friend class StreamBuffer;

  private:
    StreamBuffer* _buffer;
  };

  class StreamBuffer: public std::streambuf
  {
  public:
    typedef unsigned int Size;
    StreamBuffer();
    ~StreamBuffer();

  /// API to override.
  protected:
    friend class IOStream;

    /// The buffer where to write.
    virtual elle::Buffer write_buffer() = 0;

    /// The buffer with the next available data.
    virtual elle::Buffer read_buffer()  = 0;

    /// Synchronize the write buffer to the underlying implementation.
    virtual void   flush(Size size);

  /// std::streambuf interface.
  protected:
    virtual int underflow();
    virtual int overflow(int c);
    virtual int sync();
  };

  /// Simple implementation of a streambuf with local buffers.
  class PlainStreamBuffer: public StreamBuffer
  {
  public:
    typedef unsigned int Size;
    PlainStreamBuffer();
    ~PlainStreamBuffer();

  protected:
    friend class IOStream;
    virtual Size read (char* buffer, Size size) = 0;
    virtual void write(char* buffer, Size size) = 0;

    virtual Buffer write_buffer();
    virtual Buffer read_buffer();
    virtual void   flush(unsigned int size);

  private:
    static const int _bufsize = 512;
    char _ibuf[_bufsize];
    char _obuf[_bufsize];
  };

}

#endif
