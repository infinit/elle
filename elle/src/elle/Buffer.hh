#ifndef ELLE_BUFFER_HH
# define ELLE_BUFFER_HH

# include <elle/IOStream.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/serialize/construct.hh>
# include <elle/serialize/fwd.hh>
# include <elle/types.hh>

# include <iosfwd>
# include <memory>
# include <boost/operators.hpp>
# include <limits>

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
    private boost::totally_ordered<Buffer>
  {
  /*------.
  | Types |
  `------*/
  public:
    /// Size of a Buffer.
    typedef size_t Size;
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
    /// Buffer is not copyable
    Buffer(Buffer const&) = delete;

  /*------------------.
  | Memory management |
  `------------------*/
  public:
    /// Size of the buffer.
    ELLE_ATTRIBUTE_Rw(Size, size);
    /// Size of the underlying allocated memory.
    ELLE_ATTRIBUTE_Rw(Size, capacity);
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

  public:
    static const size_t max_size = std::numeric_limits<size_t>::max();

  /*-----------.
  | Operations |
  `-----------*/
  public:
    /// Append a copy of the data to the end of the buffer.
    void
    append(void const* data,
           size_t size);

  /*---------------------.
  | Relational Operators |
  `---------------------*/
  public:
    bool
    operator ==(Buffer const& other) const;
    bool
    operator ==(ConstWeakBuffer const& other) const;
    bool
    operator ==(std::string const& other) const;
    bool
    operator ==(char const* other) const;
    bool
    operator <(Buffer const& other) const;

  /*-----------.
  | Properties |
  `-----------*/
  public:
    /// Whether the size is 0.
    bool
    empty() const;
    /// The content of the buffer as a string.
    std::string
    string() const;

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
  | Iterable |
  `---------*/
  public:
    typedef Byte* iterator;
    typedef Byte const* const_iterator;
    const Byte*
    begin() const;
    const Byte*
    end() const;
    Byte*
    begin();
    Byte*
    end();

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
  class ConstWeakBuffer:
    private boost::totally_ordered<ConstWeakBuffer>
  {
  /*-------------.
  | Construction |
  `-------------*/
  public:
    /// ConstWeakBuffer with null memory segment and size.
    ConstWeakBuffer();
    /// WeakBuffer for the given memory segment.
    ConstWeakBuffer(const void* data, size_t size);
    /// WeakBuffer with \a data content.
    ConstWeakBuffer(std::string const& data) /* implicit */;
    /// WeakBuffer with \a data content.
    ConstWeakBuffer(char const* data) /* implicit */;
    /// ConstWeakBuffer for the given Buffer content.
    ConstWeakBuffer(Buffer const& buffer) /* implicit */;
    /// ConstWeakBuffer copy.
    ConstWeakBuffer(ConstWeakBuffer const& other);
    /// ConstWeakBuffer move.
    ConstWeakBuffer(ConstWeakBuffer&& other);
  private:
    /// ConstWeakBuffer cannot take ownership of memory.
    ConstWeakBuffer(Buffer&&);

  /*--------.
  | Content |
  `--------*/
  public:
    /// Get byte at position \a i.
    Byte&
    operator[] (unsigned i);

    /// Get byte at position \a i.
    Byte
    operator[] (unsigned i) const;
    /// Size of the buffer.
    ELLE_ATTRIBUTE_R(size_t, size);
    /// Buffer constant data.
    ELLE_ATTRIBUTE_R(const Byte*, contents);

  /*---------------------.
  | Relational Operators |
  `---------------------*/
  public:
    bool
    operator ==(ConstWeakBuffer const& other) const;
    bool
    operator <(ConstWeakBuffer const& other) const;
    bool
    operator ==(std::string const& other) const;

  /*-----------.
  | Properties |
  `-----------*/
  public:
    /// Whether the size is 0.
    bool
    empty() const;
    /// The content of the buffer as a string.
    std::string
    string() const;

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
    typedef Byte const* const_iterator;
    const Byte*
    begin() const;
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

  /// A ConstWeakBuffer with mutable data.
  class WeakBuffer:
    public ConstWeakBuffer, private boost::totally_ordered<WeakBuffer>
  {
  /*-------------.
  | Construction |
  `-------------*/
  public:
    /// WeakBuffer with null memory segment and size.
    WeakBuffer();
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
    Byte*
    mutable_contents() const;

  /*---------.
  | Iterable |
  `---------*/
  public:
    typedef Byte* iterator;
    Byte*
    begin();
    Byte*
    end();
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
              ConstWeakBuffer const& buffer);

}

# include <elle/Buffer.hxx>

#endif
