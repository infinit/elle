#ifndef ELLE_BUFFER_HH
# define ELLE_BUFFER_HH

# include <iosfwd>
# include <limits>
# include <memory>

# include <boost/operators.hpp>

# include <elle/IOStream.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
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

  class WeakBuffer;

  /*-------.
  | Buffer |
  `-------*/

  /// @brief A memory zone.
  ///
  /// The Buffer owns the pointed memory at every moment.
  ///
  /// @see WeakBuffer for a buffer that doesn't own the memory.
  class ELLE_API Buffer:
    private boost::totally_ordered<Buffer>
  {
  /*------.
  | Types |
  `------*/
  public:
    /// Size of a Buffer.
    typedef uint64_t Size;
    /// Data owned by a Buffer:
    typedef std::unique_ptr<Byte, detail::MallocDeleter> ContentPtr;
    /// Content owned by a Buffer: data and size.
    typedef std::pair<ContentPtr, Size> ContentPair;

  /*-------------.
  | Construction |
  `-------------*/
  public:
    /// An empty buffer.
    Buffer();
    /// An uninitialized buffer of the specified size.
    template <
      typename T,
      std::enable_if_t<std::is_integral<T>::value, int> = 0>
    Buffer(T size);
    /// A buffer containing a copy of the given data.
    Buffer(void const* data, Size size);
    /// A buffer containing a copy of the given data.
    Buffer(std::string const& data);
    /// A copy of the literal string.
    explicit
    Buffer(char const* data);
    /// A buffer with the content of the moved buffer.
    Buffer(Buffer&& other);
    /// A copy of the source buffer.
    Buffer(Buffer const& source);
    /// A copy of the source buffer.
    explicit
    Buffer(ConstWeakBuffer const& source);
    /// Steal the content of the moved buffer.
    Buffer&
    operator = (Buffer&& other);
    /// Free owned memory.
    ~Buffer();

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
    static Size _next_size(Size);

  public:
    static constexpr Size max_size = std::numeric_limits<Size>::max();

  /*-----------.
  | Operations |
  `-----------*/
  public:
    /// Append a copy of the data to the end of the buffer.
    void
    append(void const* data, Size size);
    /// Drop a number of bytes.
    /// Costly, as it memmoves the remaing bytes.
    void
    pop_front(Size size = 1);

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
    /// Get byte at position \a i.
    Byte&
    operator [](unsigned i);
    /// Get byte at position \a i.
    Byte
    operator [](unsigned i) const;

  /*--------------.
  | Serialization |
  `--------------*/
  public:
    /// Construct an output streambuf from the buffer.
    std::streambuf* ostreambuf();

    /// Construct an input streambuf from the buffer.
    std::streambuf* istreambuf() const;

    /// Construct an input stream buffer from the concatenation of two buffers.
    std::streambuf* istreambuf_combine(const Buffer& b) const;
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

  ELLE_API
  std::ostream&
  operator <<(std::ostream& stream,
              Buffer const& buffer);


  /*----------------.
  | ConstWeakBuffer |
  `----------------*/

  /// @brief A C array pointer and its size.
  ///
  /// This class is NOTHING but a glorified std::pair<char*, int> used to
  /// represent C-style buffers as one entity, with some useful shortcuts and
  /// facilities.  It has no intelligence or memory managment whatsoever, and
  /// shouldn't have any.
  class ELLE_API ConstWeakBuffer:
    private boost::totally_ordered<ConstWeakBuffer>
  {
  /*------.
  | Types |
  `------*/
  public:
    using Self = ConstWeakBuffer;

    /// Size of a Buffer.
    using Size = Buffer::Size;

  /*-------------.
  | Construction |
  `-------------*/
  public:
    /// ConstWeakBuffer with null memory segment and size.
    ConstWeakBuffer();
    /// ConstWeakBuffer for the given Buffer content.
    ConstWeakBuffer(Buffer const& buffer) /* implicit */;
    /// ConstWeakBuffer copy.
    ConstWeakBuffer(ConstWeakBuffer const& other);
    /// ConstWeakBuffer move.
    ConstWeakBuffer(ConstWeakBuffer&& other);
    ConstWeakBuffer& operator = (ConstWeakBuffer const& other) = default;

    /// WeakBuffer for the given memory segment.
    ConstWeakBuffer(const void* data, Size size);
    /// WeakBuffer with \a data content.
    ConstWeakBuffer(std::string const& data) /* implicit */;
    /// WeakBuffer with \a data content.
    ConstWeakBuffer(char const* data) /* implicit */;
  private:
    /// ConstWeakBuffer cannot take ownership of memory.
    ConstWeakBuffer(Buffer&&) = delete;

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
    /// A subset of this buffer.
    ConstWeakBuffer
    range(int start) const;
    /// A subset of this buffer.
    ConstWeakBuffer
    range(int start, int end) const;
    /// Size of the buffer.
    ELLE_ATTRIBUTE_R(Size, size);
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
    operator ==(std::string const& data) const;
    bool
    operator ==(char const* data) const;

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
    /// Construct an input streambuf from the buffer.
    std::streambuf* istreambuf() const;

    // Note: no ostreambuf equivalent, as the buffer is const

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


  /*-----------.
  | WeakBuffer |
  `-----------*/

  /// A ConstWeakBuffer with mutable data.
  class ELLE_API WeakBuffer:
    public ConstWeakBuffer, private boost::totally_ordered<WeakBuffer>
  {
  /*------.
  | Types |
  `------*/
    using Super = ConstWeakBuffer;
    using Self = WeakBuffer;

  public:
  /*-------------.
  | Construction |
  `-------------*/
  public:
    /// WeakBuffer with null memory segment and size.
    WeakBuffer();
    /// WeakBuffer copy.
    WeakBuffer(WeakBuffer const& other);
    /// WeakBuffer move.
    WeakBuffer(WeakBuffer&& other);

    /// WeakBuffer for the given memory segment.
    WeakBuffer(void* data, Size size);
    /// WeakBuffer for the given Buffer content.
    WeakBuffer(Buffer const& buffer) /* implicit */;
    /// WeakBuffer assignement.
    WeakBuffer& operator = (WeakBuffer const& other) = default;

  private:
    /// WeakBuffer cannot take ownership of memory.
    WeakBuffer(Buffer&&) = delete;

  /*--------.
  | Content |
  `--------*/
  public:
    Byte*
    mutable_contents() const;
    /// A subset of this buffer.
    WeakBuffer
    range(int start) const;
    /// A subset of this buffer.
    WeakBuffer
    range(int start, int end) const;


  /*---------.
  | Iterable |
  `---------*/
  public:
    typedef Byte* iterator;
    Byte*
    begin();
    Byte*
    end();

  /*--------------.
  | Serialization |
  `--------------*/

  public:
    /// Construct an output streambuf from the buffer.
    std::streambuf* ostreambuf();

    /// Construct an input streambuf from the buffer.
    std::streambuf* istreambuf() const;
  };

  /*----------.
  | Operators |
  `----------*/

  ELLE_API
  std::ostream&
  operator <<(std::ostream& stream,
              ConstWeakBuffer const& buffer);

}

/*-----.
| Hash |
`-----*/

namespace std
{
  template<>
  struct ELLE_API hash<elle::ConstWeakBuffer>
  {
  public:
    elle::Buffer::Size operator()(elle::ConstWeakBuffer const& buffer) const;
  };

  template<>
  struct ELLE_API hash<elle::Buffer>
  {
  public:
    elle::Buffer::Size operator()(elle::Buffer const& buffer) const;
  };
}

# include <elle/Buffer.hxx>

#endif
