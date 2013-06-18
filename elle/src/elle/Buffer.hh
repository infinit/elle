#ifndef ELLE_BUFFER_HH
# define ELLE_BUFFER_HH

# include <elle/IOStream.hh>
# include <elle/Orderable.hh>
# include <elle/attribute.hh>
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
    public elle::Orderable<Buffer>
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
    Buffer(void const* data,
           size_t size);
    /// A buffer with the content of the moved buffer.
    Buffer(Buffer&& other);
    /// Steal the content of the moved buffer.
    Buffer&
    operator = (Buffer&& other);
    /// Free owned memory.
    ~Buffer();
  private:
    /// Buffer is not copyable
    Buffer(Buffer const&);

  /*------------------.
  | Memory management |
  `------------------*/
  public:
    /// Size of the buffer.
    ELLE_ATTRIBUTE_Rw(size_t, size);
    /// Size of the underlying allocated memory.
    ELLE_ATTRIBUTE_Rw(size_t, capacity);
    /// Buffer data.
    ELLE_ATTRIBUTE_R(Byte*, contents);
    /// Buffer mutable data.
    Byte*
    mutable_contents() const;
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
    static size_t _next_size(size_t);

  /*-----------.
  | Operations |
  `-----------*/
  public:
    /// Append a copy of the data to the end of the buffer.
    void
    append(void const* data,
           size_t size);

  /*----------.
  | Orderable |
  `----------*/
  private:
    friend class Orderable<elle::Buffer>;
    Orderable<elle::Buffer>::Order
    _order(Buffer const& other) const;

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


  /*-----------.
  | WeakBuffer |
  `-----------*/

  /// @brief A C array pointer and its size.
  ///
  /// This class is NOTHING but a glorified std::pair<char*, int> used to
  /// represent C-style buffers as one entity, with some useful shortcuts and
  /// facilities.  It has no intelligence or memory managment whatsoever, and
  /// shouldn't have any.
  class WeakBuffer:
    public elle::Orderable<WeakBuffer>
  {
  /*-------------.
  | Construction |
  `-------------*/
  public:
    /// WeakBuffer for the given memory segment.
    WeakBuffer(void* data, size_t size);
    /// WeakBuffer for the given Buffer content.
    WeakBuffer(Buffer const& buffer) /* implicit */;
    /// WeakBuffer copy.
    WeakBuffer(WeakBuffer const& other);
    /// WeakBuffer move.
    WeakBuffer(WeakBuffer&& other);
  private:
    /// WeakBuffer cannot take ownership of memory.
    WeakBuffer(Buffer&&);

  /*--------.
  | Content |
  `--------*/
  public:
    /// Size of the buffer.
    ELLE_ATTRIBUTE_R(size_t, size);
    /// Buffer constant data.
    ELLE_ATTRIBUTE_R(Byte*, contents);
    /// Buffer mutable data.
    Byte*
    mutable_contents() const;

  /*----------.
  | Orderable |
  `----------*/
  private:
    friend class elle::Buffer;
    friend class Orderable<elle::WeakBuffer>;
    Orderable<elle::WeakBuffer>::Order
    _order(WeakBuffer const& other) const;

  /*--------------.
  | Serialization |
  `--------------*/
  public:
    InputBufferArchive
    reader() const;

  /*---------.
  | Iterable |
  `---------*/
  public:
    Byte*
    begin();
    const Byte*
    begin() const;
    Byte*
    end();
    const Byte*
    end() const;

  /*---------.
  | Dumpable |
  `---------*/
  public:
    // XXX[to remove in the future, if we use DumpArchives]
    void
    dump(const Natural32 shift = 0) const;
  };

  /*-------------------.
  | OutputStreamBuffer |
  `-------------------*/

  /// A StreamBuffer to write into a Buffer.
  class OutputStreamBuffer:
    public StreamBuffer
  {
  public:
    OutputStreamBuffer(Buffer& buffer);

  protected:
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
    size_t _old_size;
    Buffer& _buffer;
  };

  /*------------------.
  | InputStreamBuffer |
  `------------------*/

  template <typename BufferType>
  class InputStreamBuffer:
    public StreamBuffer
  {
  public:
    explicit
    InputStreamBuffer(BufferType const& buffer);

  protected:
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
    BufferType const& _buffer;
    bool _read;
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
