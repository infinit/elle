#include <elle/Buffer.hh>

#include <algorithm> // std::count_if
#include <iomanip>
#include <iostream>
#include <locale> // std::isprint

#include <elle/Exception.hh>
#include <elle/IOStream.hh>
#include <elle/assert.hh>
#include <elle/finally.hh>
#include <elle/format/hexadecimal.hh>
#include <elle/io/Dumpable.hh>
#include <elle/log.hh>

ELLE_LOG_COMPONENT("elle.Buffer");

#define ELLE_BUFFER_INITIAL_SIZE (sizeof(void*))

namespace elle
{
  /*-------------------.
  | OutputStreamBuffer |
  `-------------------*/

  /// A StreamBuffer to write into a Buffer.
  template <typename BufferType>
  class OutputStreamBuffer:
    public StreamBuffer
  {
  public:
    explicit
    OutputStreamBuffer(BufferType& buffer);

  protected:
    virtual
    WeakBuffer
    write_buffer();
    virtual
    WeakBuffer
    read_buffer();
    virtual
    void
    flush(StreamBuffer::Size size);

  private:
    size_t _old_size;
    BufferType& _buffer;
  };

  template<>
  class OutputStreamBuffer<WeakBuffer>:
    public StreamBuffer
  {
  public:
    OutputStreamBuffer(WeakBuffer& buffer);
  protected:
    virtual
    WeakBuffer
    write_buffer();
    virtual
    WeakBuffer
    read_buffer();
    virtual
    void
    flush(StreamBuffer::Size size);
  private:
    bool _buffer_returned;
    WeakBuffer& _buffer;
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
    void add(BufferType const& buffer);
  protected:
    virtual
    WeakBuffer
    write_buffer();
    virtual
    WeakBuffer
    read_buffer();
    virtual
    void
    flush(StreamBuffer::Size size);

  private:
    std::vector<BufferType const*> _buffers;
    int _pos;
  };

  /*-------.
  | Buffer |
  `-------*/

  // Note that an empty buffer has a valid pointer to a memory region with
  // a size of zero.
  Buffer::Buffer()
    : _size(0)
    , _capacity(ELLE_BUFFER_INITIAL_SIZE)
    , _contents(static_cast<Byte*>(malloc(ELLE_BUFFER_INITIAL_SIZE)))
  {
    if (this->_contents == nullptr)
      throw std::bad_alloc();
  }

  Buffer::Buffer(void const* data, Buffer::Size size)
    : _size(0)
    , _capacity(0)
    , _contents(nullptr)
  {
    if (size == 0)
    {
      this->_capacity = ELLE_BUFFER_INITIAL_SIZE;
      if ((this->_contents =
           static_cast<Byte*>(::malloc(_capacity))) == nullptr)
        throw std::bad_alloc();
    }
    else
      this->append(data, size);
  }

  Buffer::Buffer(char const* data)
    : Buffer(data, strlen(data))
  {}

  Buffer::Buffer(std::string const& data)
    : Buffer(data.c_str(), data.size())
  {}

  Buffer::Buffer(Buffer&& other)
    : _size(0)
    , _capacity(0)
    , _contents(nullptr)
  {
    (*this) = std::move(other);
  }

  Buffer::Buffer(Buffer const& source)
    : _size(source._size)
    , _capacity(source._size)
    , _contents(static_cast<Byte*>(::malloc(this->_capacity)))
  {
    if (!this->_contents)
      throw std::bad_alloc();
    memcpy(this->_contents, source._contents, this->_size);
  }

  Buffer::Buffer(ConstWeakBuffer const& source)
    : Buffer(source.contents(), source.size())
  {}

  Buffer&
  Buffer::operator = (Buffer&& other)
  {
    ::free(this->_contents);
    this->_size = other._size;
    this->_capacity = other._capacity;
    this->_contents = other._contents;
    // XXX: this cost a lot !
    other._contents = static_cast<Byte*>(malloc(ELLE_BUFFER_INITIAL_SIZE));
    other._size = 0;
    other._capacity = ELLE_BUFFER_INITIAL_SIZE;
    return *this;
  }

  Buffer::~Buffer()
  {
    ::free(this->_contents);
  }

  void
  Buffer::capacity(boost::call_traits<Buffer::Size>::param_type capacity_)
  {
    Buffer::Size capacity = capacity_;
    if (capacity < ELLE_BUFFER_INITIAL_SIZE)
      capacity = ELLE_BUFFER_INITIAL_SIZE;
    void* tmp = ::realloc(this->_contents, capacity);
    if (tmp == nullptr)
      throw std::bad_alloc();
    this->_contents = static_cast<Byte*>(tmp);
    this->_capacity = capacity;
    this->_size = std::min(this->_size, capacity);
  }

  void Buffer::append(void const* data, Buffer::Size size)
  {
    ELLE_ASSERT(data != nullptr || size == 0);

    Buffer::Size old_size = this->_size;
    this->size(this->_size + size);
    /// XXX some implementations of memmove does not check for memory overlap
    memmove(this->_contents + old_size, data, size);
    // std::uninitialized_copy(
    //   static_cast<Byte const*>(data),
    //   static_cast<Byte const*>(data) + size,
    //   this->_contents + old_size
  }

  void
  Buffer::size(boost::call_traits<Buffer::Size>::param_type size_)
  {
    Buffer::Size size = size_;
    if (this->_capacity < size)
    {
      Buffer::Size next_size = Buffer::_next_size(size);
      void* tmp = ::realloc(_contents, next_size);
      if (tmp == nullptr)
        throw std::bad_alloc();
      this->_contents = static_cast<Byte*>(tmp);
      this->_capacity = next_size;
    }
    this->_size = size;
  }

  void
  Buffer::reset()
  {
    this->size(0);
  }

  Buffer::ContentPair
  Buffer::release()
  {
    Byte* new_contents = static_cast<Byte*>(::malloc(ELLE_BUFFER_INITIAL_SIZE));
    if (new_contents == nullptr)
        throw std::bad_alloc{};

    ContentPair res{ContentPtr{this->_contents}, this->_size};

    this->_contents = new_contents;
    this->_size = 0;
    this->_capacity = ELLE_BUFFER_INITIAL_SIZE;
    return res;
  }

  const Byte*
  Buffer::begin() const
  {
    return const_cast<Buffer*>(this)->begin();
  }

  const Byte*
  Buffer::end() const
  {
    return const_cast<Buffer*>(this)->end();
  }

  Byte*
  Buffer::begin()
  {
    return this->mutable_contents();
  }

  Byte*
  Buffer::end()
  {
    return this->mutable_contents() + this->size();
  }

  void
  Buffer::dump(const Natural32 margin) const
  {
    Natural32         space = 78 - margin - io::Dumpable::Shift.length();
    String            alignment(margin, ' ');
    Natural32         i;
    Natural32         j;

    std::cout << alignment
              << "[Buffer] "
              << "address(" << static_cast<Void*>(this->_contents) << ") "
              << "size(" << std::dec << this->_size << ") "
              << "capacity(" << std::dec << this->_capacity << ")"
              << std::endl;

    // since a byte is represented by two characters.
    space = space / 2;

    // set the fill to '0'.
    std::cout.fill('0');

    // display the region.
    for (i = 0; i < (this->_size / space); i++)
      {
        std::cout << alignment << io::Dumpable::Shift;

        for (j = 0; j < space; j++)
          std::cout << std::nouppercase
                    << std::hex
                    << std::setw(2)
                    << (int)this->_contents[i * space + j];

        std::cout << std::endl;
      }

    if ((this->_size % space) != 0)
      {
        std::cout << alignment << io::Dumpable::Shift;

        for (Buffer::Size j = 0; j < (this->_size % space); j++)
          std::cout << std::nouppercase
                    << std::hex
                    << std::setw(2)
                    << (int)this->_contents[i * space + j];

        std::cout << std::endl;
      }
  }

  Buffer::Size
  Buffer::_next_size(Buffer::Size size)
  {
    if (size < 32)
      return 32;
    if (size < 4096)
      return size *= 2;
    return (size + size / 2);
  }

  bool
  Buffer::operator ==(Buffer const& ot) const
  {
    return *this == ConstWeakBuffer(ot._contents, ot._size);
  }

  bool
  Buffer::operator ==(ConstWeakBuffer const& other) const
  {
    return WeakBuffer(_contents, _size) == other;
  }

  bool
  Buffer::operator ==(std::string const& other) const
  {
    return *this == ConstWeakBuffer(other);
  }

  bool
  Buffer::operator ==(char const* other) const
  {
    return *this == ConstWeakBuffer(other);
  }

  bool
  Buffer::operator <(Buffer const& ot) const
  {
    return WeakBuffer(_contents, _size) < WeakBuffer(ot._contents, ot._size);
  }

  /*-----------.
  | Properties |
  `-----------*/

  bool
  Buffer::empty() const
  {
    return this->size() == 0;
  }

  std::string
  Buffer::string() const
  {
    return std::string(reinterpret_cast<char const*>(this->contents()),
                       this->size());
  }

  /// Get byte at position \a i.
  Byte&
  Buffer::operator [](unsigned i)
  {
    ELLE_ASSERT_LT(i, this->_size);
    return this->_contents[i];
  }

  Byte
  Buffer::operator [](unsigned i) const
  {
    return const_cast<Buffer*>(this)->operator [](i);
  }


  void
  Buffer::shrink_to_fit()
  {
    auto size =
      std::max(static_cast<Buffer::Size>(ELLE_BUFFER_INITIAL_SIZE),
               this->_size);
    if (size < this->_capacity)
    {
      void* tmp = ::realloc(_contents, size);
      if (tmp == nullptr)
        throw std::bad_alloc();
      this->_contents = static_cast<Byte*>(tmp);
      this->_capacity = size;
    }
  }


  std::streambuf*
  Buffer::ostreambuf()
  {
    return new OutputStreamBuffer<elle::Buffer>(*this);
  }

  std::streambuf*
  Buffer::istreambuf() const
  {
    return new InputStreamBuffer<elle::Buffer>(*this);
  }

  std::streambuf*
  Buffer::istreambuf_combine(const Buffer& b) const
  {
    auto res = new InputStreamBuffer<elle::Buffer>(*this);
    res->add(b);
    return res;
  }

  std::ostream&
  operator <<(std::ostream& stream,
              Buffer const& buffer)
  {
    return stream << WeakBuffer(buffer);
  }

  /*-----------.
  | WeakBuffer |
  `-----------*/
  Byte&
  WeakBuffer::operator[] (unsigned i)
  {
    ELLE_ASSERT_LT(i, this->size());
    return const_cast<Byte&>(this->contents()[i]);
  }

  std::streambuf*
  ConstWeakBuffer::istreambuf() const
  {
    return new InputStreamBuffer<elle::ConstWeakBuffer>(*this);
  }

  std::streambuf*
  WeakBuffer::ostreambuf()
  {
    return new OutputStreamBuffer<elle::WeakBuffer>(*this);
  }

  std::streambuf*
  WeakBuffer::istreambuf() const
  {
    return new InputStreamBuffer<elle::WeakBuffer>(*this);
  }


  void
  ConstWeakBuffer::dump(const Natural32 margin) const
  {
    Natural32         space = 78 - margin - io::Dumpable::Shift.length();
    String            alignment(margin, ' ');
    Natural32         i;
    Natural32         j;

    std::cout << alignment
              << "[Buffer] "
              << "address(" << static_cast<void const*>(this->_contents) << ") "
              << "size(" << std::dec << this->_size << ")"
              << std::endl;

    // since a byte is represented by two characters.
    space = space / 2;

    // set the fill to '0'.
    std::cout.fill('0');

    // display the region.
    for (i = 0; i < (this->_size / space); i++)
      {
        std::cout << alignment << io::Dumpable::Shift;

        for (j = 0; j < space; j++)
          std::cout << std::nouppercase
                    << std::hex
                    << std::setw(2)
                    << (int)this->_contents[i * space + j];

        std::cout << std::endl;
      }

    if ((this->_size % space) != 0)
      {
        std::cout << alignment << io::Dumpable::Shift;

        for (Buffer::Size j = 0; j < (this->_size % space); j++)
          std::cout << std::nouppercase
                    << std::hex
                    << std::setw(2)
                    << (int)this->_contents[i * space + j];

        std::cout << std::endl;
      }
  }

  Byte
  ConstWeakBuffer::operator[] (unsigned i) const
  {
    ELLE_ASSERT_LT(i, this->_size);
    return this->_contents[i];
  }

  WeakBuffer
  WeakBuffer::range(int start) const
  {
    return this->range(start, this->size());
  }

  ConstWeakBuffer
  ConstWeakBuffer::range(int start) const
  {
    return this->range(start, this->size());
  }

  WeakBuffer
  WeakBuffer::range(int start, int end) const
  {
    if (start < 0)
      start = this->size() + start;
    if (end < 0)
      end = this->size() + end;
    ELLE_ASSERT_LTE(start, end);
    ELLE_ASSERT_LTE(end, signed(this->size()));
    return WeakBuffer(this->mutable_contents() + start, end - start);
  }

  ConstWeakBuffer
  ConstWeakBuffer::range(int start, int end) const
  {
    return static_cast<WeakBuffer const*>(this)->range(start, end);
  }

  bool
  ConstWeakBuffer::operator ==(ConstWeakBuffer const& other) const
  {
    if (this->_size != other.size())
      return false;
    return ::memcmp(this->_contents, other.contents(), this->_size) == 0;
  }

  bool
  ConstWeakBuffer::operator <(ConstWeakBuffer const& other) const
  {
    if (this->_size == other.size())
      return ::memcmp(this->_contents, other.contents(), this->_size) < 0;
    return this->_size < other.size();
  }

  bool
  ConstWeakBuffer::operator ==(std::string const& data) const
  {
    return *this == ConstWeakBuffer(data);
  }

  bool
  ConstWeakBuffer::operator ==(char const* data) const
  {
    return *this == ConstWeakBuffer(data);
  }

  /*---------.
  | Iterable |
  `---------*/

  Byte*
  WeakBuffer::begin()
  {
    return const_cast<Byte*>(this->ConstWeakBuffer::begin());
  }

  const Byte*
  ConstWeakBuffer::begin() const
  {
    return this->contents();
  }

  Byte*
  WeakBuffer::end()
  {
    return const_cast<Byte*>(this->ConstWeakBuffer::end());
  }

  const Byte*
  ConstWeakBuffer::end() const
  {
    return this->contents() + this->size();
  }

  /*----------.
  | Operators |
  `----------*/

  static
  void
  put(std::ostream& stream,
      ConstWeakBuffer const& buffer,
      bool hex)
  {
    if (hex)
    {
      stream << "0x" << format::hexadecimal::encode(buffer);
    }
    else
      stream.write(reinterpret_cast<const char*>(buffer.contents()),
                   buffer.size());
  }

  std::ostream&
  operator <<(std::ostream& stream,
              ConstWeakBuffer const& buffer)
  {
    static int const max_length = 20;
    bool hex = stream.flags() & std::ios::hex;
    bool fixed = stream.flags() & std::ios::fixed;
    int printable = std::count_if(buffer.begin(), buffer.end(),
                                  [] (char c) { return std::isprint(c); });
    std::ios state(nullptr);
    state.copyfmt(stream);
    elle::SafeFinally restore_stream_state([&] { std::cout.copyfmt(state); });
    if (float(printable) / buffer.size() < 0.9)
      hex = true;
    if (fixed && signed(buffer.size()) > max_length)
    {
      put(stream, buffer.range(0, max_length / 2), hex);
      stream << "...";
      put(stream, buffer.range(-max_length / 2), hex);
    }
    else
      put(stream, buffer, hex);
    return stream;
  }

  /*-----------.
  | Properties |
  `-----------*/

  bool
  ConstWeakBuffer::empty() const
  {
    return this->size() == 0;
  }

  std::string
  ConstWeakBuffer::string() const
  {
    return std::string(reinterpret_cast<char const*>(this->contents()),
                       this->size());
  }


  /*------------------.
  | InputStreamBuffer |
  `------------------*/

  template <typename BufferType>
  InputStreamBuffer<BufferType>::InputStreamBuffer(BufferType const& buffer):
    _pos(0)
  {
    _buffers.push_back(&buffer);
    ELLE_DEBUG("create an InputStreamBuffer on a buffer of size %s",
               buffer.size());
  }

  template <typename BufferType>
  void
  InputStreamBuffer<BufferType>::add(BufferType const& buffer)
  {
    _buffers.push_back(&buffer);
  }

  template <typename BufferType>
  WeakBuffer
  InputStreamBuffer<BufferType>::write_buffer()
  {
    throw Exception("the buffer is in input mode");
  }

  template <typename BufferType>
  WeakBuffer
  InputStreamBuffer<BufferType>::read_buffer()
  {
    if (this->_pos < signed(this->_buffers.size()))
    {
      this->_pos++;
      return WeakBuffer((char*)_buffers[_pos-1]->contents(), _buffers[_pos-1]->size());
    }
    else
      return WeakBuffer(nullptr, 0);
  }

  template <typename BufferType>
  void
  InputStreamBuffer<BufferType>::flush(StreamBuffer::Size)
  {
    throw Exception("the buffer is in input mode");
  }

  template
  class InputStreamBuffer<Buffer>;
  template
  class InputStreamBuffer<ConstWeakBuffer>;
  template
  class InputStreamBuffer<WeakBuffer>;

  /*-------------------.
  | OutputStreamBuffer |
  `-------------------*/

  template <typename BufferType>
  OutputStreamBuffer<BufferType>::OutputStreamBuffer(BufferType& buffer):
    _old_size(buffer.size()),
    _buffer(buffer)
  {}

  template <typename BufferType>
  WeakBuffer
  OutputStreamBuffer<BufferType>::write_buffer()
  {
    _buffer.capacity(this->_old_size + 512);
    ELLE_DEBUG("%s: grow buffer capacity from %s to %s bytes",
               *this, this->_old_size, this->_buffer.capacity());
    return WeakBuffer(
      (char*)_buffer.mutable_contents() + this->_old_size,
      512
      );
  }

  template <typename BufferType>
  WeakBuffer
  OutputStreamBuffer<BufferType>::read_buffer()
  {
    throw Exception("buffer is in output mode");
  }

  template <typename BufferType>
  void
  OutputStreamBuffer<BufferType>::flush(StreamBuffer::Size size)
  {
    ELLE_DEBUG("%s: flush buffer stream size: %s", *this, size);
    _old_size += size;
    _buffer.size(_old_size);
  }

  OutputStreamBuffer<WeakBuffer>::OutputStreamBuffer(WeakBuffer& buffer):
    _buffer_returned(false),
    _buffer(buffer)
  {}

  WeakBuffer
  OutputStreamBuffer<WeakBuffer>::write_buffer()
  {
    if (_buffer_returned)
      throw Exception("the buffer is full and cannot grow");
    _buffer_returned = true;
    return _buffer;
  }

  WeakBuffer
  OutputStreamBuffer<WeakBuffer>::read_buffer()
  {
    throw Exception("the buffer is in output mode");
  }

  void
  OutputStreamBuffer<WeakBuffer>::flush(StreamBuffer::Size size)
  {
    ELLE_DEBUG("Flush buffer stream size: %s", size);
  }

  template
  class OutputStreamBuffer<Buffer>;

  /*----------.
  | Operators |
  `----------*/

  std::ostream&
  operator <<(std::ostream& stream,
              ConstWeakBuffer const& buffer);
}

/*-----.
| Hash |
`-----*/

namespace std
{
  elle::Buffer::Size
  hash<elle::ConstWeakBuffer>::operator()(elle::ConstWeakBuffer const& buffer) const
  {
    return std::hash<std::string>()(buffer.string());
  }

  elle::Buffer::Size
  hash<elle::Buffer>::operator()(elle::Buffer const& buffer) const
  {
    return hash<elle::ConstWeakBuffer>()(buffer);
  }
}
