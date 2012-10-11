#ifndef ELLE_BUFFER_HH
# define ELLE_BUFFER_HH

# include <memory>

# include <elle/serialize/fwd.hh>
# include <elle/types.hh>

namespace elle
{
  namespace detail
  {
    struct MallocDeleter
    {
      void operator() (void* data);
    };
  }

  class InputBufferArchive;
  class OutputBufferArchive;
  class WeakBuffer;

  /// @brief Manage a memory zone.
  ///
  /// Note that this class owns the pointed memory at every moment.
  ///
  /// @see WeakBuffer for a buffer that doesn't own the data
  class Buffer
  {
    friend class serialize::Serializer<Buffer>;
  public:
    typedef std::unique_ptr<Byte, detail::MallocDeleter> ContentPtr;
    typedef std::pair<ContentPtr, size_t>                ContentPair;

  private:
    Byte*       _contents;
    size_t      _size;
    size_t      _buffer_size;

  public:
    /// Empty buffer
    Buffer();

    /// A buffer of a specify size.
    explicit
    Buffer(size_t size);

    /// Steal a pointer as internal buffer
    explicit
    Buffer(ContentPair&& pair);

    /// Copy a buffer
    Buffer(Byte const* data,
           size_t size);

    /// Buffer class is moveable
    Buffer(Buffer&& other);
    Buffer&
    operator =(Buffer&& other);

    /// Buffer class is not virtual
    ~Buffer();

    // Buffer is not copyable
    Buffer(Buffer const&) = delete;
    Buffer&
    operator =(Buffer const&) = delete;

  public:
    /// Append a copy of the data to the end of the buffer.
    void
    append(void const* data,
           size_t size);

    /// Properties for the size and the buffer contents
    void          size(size_t size);
    size_t        size() const { return this->_size; }
    Byte const*   contents() const { return this->_contents; }
    Byte*         mutable_contents() const { return this->_contents; }

    /// Reset the size to zero.
    void
    reset() { this->size(0); }

    /// Release internal memory.
    ContentPair
    release();

    /// Binary serialization shorcut.
    OutputBufferArchive
    writer();

    InputBufferArchive
    reader() const;

    void
    dump(const Natural32 shift = 0) const;

    bool
    operator <(Buffer const& other) const;
    bool
    operator ==(Buffer const& other) const;

  private:
    static size_t _next_size(size_t);
  };


  /// @brief A C array pointer and its size.
  ///
  /// This class is NOTHING but a glorified std::pair<char*, int> used to
  /// represent C-style buffers as one entity, with some useful shortcuts and
  /// facilities.  It has no intelligence or memory managment whatsoever, and
  /// shouldn't have any.
  ///
  class WeakBuffer
  {
  private:
    Byte*     _contents;
    size_t    _size;

  public:
    WeakBuffer(void* data, size_t size)
      : _contents(static_cast<Byte*>(data))
      , _size(size)
    {}

    WeakBuffer(Buffer const& buffer)
      : _contents(buffer.mutable_contents())
      , _size(buffer.size())
    {}

    WeakBuffer(WeakBuffer const& buffer)
      : _contents(buffer.mutable_contents())
      , _size(buffer.size())
    {}

    WeakBuffer(WeakBuffer&& other)
      : _contents(other._contents)
      , _size(other._size)
    {}

    size_t        size() const              { return this->_size; }
    Byte const*   contents() const          { return this->_contents; }
    Byte*         mutable_contents() const  { return this->_contents; }

    InputBufferArchive
    reader() const;
  };

}

# include "Buffer.hxx"

#endif
