#ifndef ELLE_BUFFER_HH
# define ELLE_BUFFER_HH

# include <elle/Comparable.hh>
# include <elle/operator.hh>
# include <elle/serialize/construct.hh>
# include <elle/serialize/fwd.hh>
# include <elle/types.hh>

# include <iosfwd>
# include <memory>

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

  /*-------.
  | Buffer |
  `-------*/

  /// @brief A memory zone.
  ///
  /// The Buffer owns the pointed memory at every moment.
  ///
  /// @see WeakBuffer for a buffer that doesn't own the memory.
  class Buffer:
    public elle::Comparable<Buffer>,
    public elle::Comparable<WeakBuffer>
  {

  /*------.
  | Types |
  `------*/
  public:
    /// Data owned by a Buffer:
    typedef std::unique_ptr<Byte, detail::MallocDeleter> ContentPtr;
    /// Content owned by a Buffer: data and size.
    typedef std::pair<ContentPtr, size_t> ContentPair;

  /*-------------.
  | Construction |
  `-------------*/
  public:
    /// An empty buffer.
    Buffer();
    /// A buffer of a specify size.
    explicit
    Buffer(size_t size);
    /// A bufferSteal a pointer as internal buffer
    explicit
    Buffer(ContentPair&& pair);
    /// A buffer containing a copy of the given data.
    Buffer(Byte const* data,
           size_t size);
    /// A buffer with the content of the moved buffer.
    Buffer(Buffer&& other);
    Buffer&
    operator =(Buffer&& other);
    /// Free owned memory.
    ~Buffer();
    /// Buffer is not copyable
    Buffer(Buffer const&) = delete;
    /// Buffer is not assignable
    Buffer&
    operator =(Buffer const&) = delete;


  /*------------------.
  | Memory management |
  `------------------*/
  public:
    /// Current size of the buffer.
    size_t
    size() const;
    /// Current underlying allocated memory.
    size_t
    capacity() const;
    /// Buffer constant data.
    Byte const*
    contents() const;
    Byte*
    /// Buffer mutable data.
    mutable_contents() const;
    void
    size(size_t size);
    /// Reset the size to zero.
    void
    reset();
    /// Release internal memory.
    ContentPair
    release();
    /// Shrink the capacity to fit the size if needed.
    void
    shrink_to_fit();
  private:
    Byte*       _contents;
    size_t      _size;
    size_t      _buffer_size;
    static size_t _next_size(size_t);

  /*-----------.
  | Operations |
  `-----------*/
  public:
    /// Append a copy of the data to the end of the buffer.
    void
    append(void const* data,
           size_t size);

  /*----------------------.
  | Comparable, Orderable |
  `----------------------*/
  public:
    bool
    operator <(Buffer const& other) const;
    bool
    operator <=(Buffer const& other) const;
    bool
    operator ==(Buffer const& other) const;
    ELLE_OPERATOR_GT(Buffer);
    ELLE_OPERATOR_GTE(Buffer);
    bool
    operator <(WeakBuffer const& other) const;
    bool
    operator <=(WeakBuffer const& other) const;
    bool
    operator ==(WeakBuffer const& other) const;
    ELLE_OPERATOR_GT(WeakBuffer);
    ELLE_OPERATOR_GTE(WeakBuffer);

  /*--------------.
  | Serialization |
  `--------------*/
  public:
    friend struct serialize::Serializer<Buffer>;
    /// Load constructor.
    ELLE_SERIALIZE_CONSTRUCT_DECLARE(Buffer);
    /// Binary serialization write shorcut.
    OutputBufferArchive
    writer();
    /// Binary serialization read shorcut.
    InputBufferArchive
    reader() const;

  /*---------.
  | Dumpable |
  `---------*/
  public:
    // XXX[to remove in the future, if we use DumpArchives]
    void
    dump(const Natural32 shift = 0) const;
  };

  std::ostream&
  operator <<(std::ostream& stream,
              Buffer const& buffer);

//
// ---------- WeakBuffer ------------------------------------------------------
//

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

    WeakBuffer(Buffer&&) = delete;

    WeakBuffer(WeakBuffer const& other)
      : _contents(other._contents)
      , _size(other._size)
    {}

    WeakBuffer(WeakBuffer&& other)
      : _contents(other._contents)
      , _size(other._size)
    {
      other._contents = nullptr;
      other._size = 0;
    }

    size_t        size() const              { return this->_size; }
    Byte const*   contents() const          { return this->_contents; }
    Byte*         mutable_contents() const  { return this->_contents; }

    InputBufferArchive
    reader() const;

    // XXX[to remove in the future, if we use DumpArchives]
    void
    dump(const Natural32 shift = 0) const;

    bool
    operator <(WeakBuffer const& other) const;
    bool
    operator <=(WeakBuffer const& other) const;
    bool
    operator ==(WeakBuffer const& other) const;
    ELLE_OPERATOR_GT(WeakBuffer);
    ELLE_OPERATOR_GTE(WeakBuffer);
    ELLE_OPERATOR_NEQ(WeakBuffer);
    bool
    operator <(Buffer const& other) const;
    bool
    operator <=(Buffer const& other) const;
    bool
    operator ==(Buffer const& other) const;
    ELLE_OPERATOR_GT(Buffer);
    ELLE_OPERATOR_GTE(Buffer);
    ELLE_OPERATOR_NEQ(Buffer);
  };

  /*----------.
  | Operators |
  `----------*/

  std::ostream&
  operator <<(std::ostream& stream,
              WeakBuffer const& buffer);

}

# include <elle/Buffer.hxx>

#endif
