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

  /// Wrapper around std::iostream in charge of synchronizing and destroying
  /// underlying buffer on destruction.
  class ELLE_API IOStream
    : public std::iostream
  {
  public:
    /// Create an IOStream for the given std::streambuf.
    ///
    /// @param buffer An un-managed pointer to a buffer.
    IOStream(std::streambuf* buffer);
    /// Create an IOStream by taking the ownership of the underlying buffer.
    IOStream(IOStream&& buffer);
    /// Destroy the IOStream.
    ///
    /// Synchronize the underlying buffer and destroy it.
    ~IOStream() override;

  protected:
    std::streambuf* _buffer;
  };

  /// An abstract implementation of std::streambuf.
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

    /// Return the buffer where to write.
    ///
    /// @return A WeakBuffer where to write.
    virtual
    WeakBuffer
    write_buffer() = 0;

    /// The buffer with the next available data.
    ///
    /// @return A WeakBuffer from where to read data.
    virtual
    WeakBuffer
    read_buffer()  = 0;

    /// Synchronize the write buffer to the underlying implementation.
    ///
    /// Default implementation is an noop.
    ///
    /// @param size XXX[doc].
    virtual
    void
    flush(Size size);

  /// std::streambuf interface.
  protected:
    /// Ensures that at least one character is available in the input area.
    ///
    /// @see std::basic_streambuf::underflow.
    ///
    /// @returns EOF If read_buffer returned an empty buffer, otherwise an int
    ///              representing the address of the beginning of the content.
    int
    underflow() override;

    /// Ensures that there is space at the put area for at least one character.
    ///
    /// @see std::basic_streambuf::overflow.
    ///
    /// @param c The character to store in the put area
    /// @returns Non-EOF value.
    int
    overflow(int c) override;

    /// Synchronize the contents in the buffer with those of the associated
    /// character sequence.
    ///
    /// Call ::flush if (this->pptr() - this->pbase()) > 0.
    ///
    /// @see std::basic_streambuf::sync.
    ///
    /// @returns 0 on success, -1 on failure.
    int
    sync() override;
  };

  /// Simple implementation of a streambuf with local buffers.
  ///
  /// This use statically allocated internal buffers of size 4096.
  class ELLE_API PlainStreamBuffer
    : public StreamBuffer
  {
  public:
    using Size = StreamBuffer::Size;
    PlainStreamBuffer();
    ~PlainStreamBuffer() override;

  protected:
    friend class IOStream;
    /// Perform read.
    ///
    /// @param buffer The buffer to store data read.
    /// @param size The maximum number of bytes to read.
    /// @returns The number of bytes read.
    virtual
    Size
    read(char* buffer, Size size) = 0;
    /// Perform writte.
    ///
    /// @param buffer The buffer containing the data to write.
    /// @param size The number of bytes to write.
    virtual
    void
    write(char* buffer, Size size) = 0;
    /// Return a WeakBuffer to the underlying _obuf.
    WeakBuffer
    write_buffer() override;
    /// Return a WeakBuffer to the data read (calling StreamBuffer::read).
    WeakBuffer
    read_buffer() override;
    /// Synchronize the StreamBuffer by writing (calling StreamBuffer::write)
    /// the _ibuf.
    void
    flush(Size size) override;

  private:
    static const int _bufsize = (1 << 12); // == 4096 bytes.
    char _ibuf[_bufsize];
    char _obuf[_bufsize];
  };

  /// Like PlainStreamBuffer but with an configurable size.
  ///
  /// In this version, underlying buffers are dynamically allocated by the
  /// constructor.
  ///
  /// N.B. It can't be resized afterward.
  class ELLE_API DynamicStreamBuffer
    : public StreamBuffer
  {
  public:
    using Size = StreamBuffer::Size;
    using Byte = unsigned char;

    /// Construct a DynamicStreamBuffer of a given size.
    ///
    /// @param size Size of the underlying buffers.
    DynamicStreamBuffer(Size size);
    ~DynamicStreamBuffer() override;

  protected:
    friend class IOStream;
    // @see PlainStreamBuffer::read.
    virtual
    Size
    read(char *buffer, Size size) = 0;
    // @see PlainStreamBuffer::write.
    virtual
    void
    write(char *buffer, Size size) = 0;
    // @see PlainStreamBuffer::read_buffer.
    WeakBuffer
    read_buffer() override;
    // @see PlainStreamBuffer::write_buffer.
    WeakBuffer
    write_buffer() override;
    /// @see PlainStreamBuffer::flush.
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
