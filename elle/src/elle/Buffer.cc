#include "Buffer.hh"

#include <elle/assert.hh>
#include <elle/Exception.hh>
#include <elle/IOStream.hh>
#include <elle/log.hh>

#include <elle/format/hexadecimal.hh>

#include <elle/io/Dumpable.hh>

#include <iomanip>
#include <iostream>

ELLE_LOG_COMPONENT("elle.Buffer");

#define ELLE_BUFFER_INITIAL_SIZE (sizeof(void*))

namespace elle
{
  void detail::MallocDeleter::operator ()(void* data)
  {
    ::free(data);
  }

//
// ---------- Buffer ----------------------------------------------------------
//

  // Note that an empty buffer has a valid pointer to a memory region with
  // a size of zero.
  Buffer::Buffer():
    _size(0),
    _capacity(ELLE_BUFFER_INITIAL_SIZE),
    _contents(static_cast<Byte*>(malloc(ELLE_BUFFER_INITIAL_SIZE)))
  {
    if (this->_contents == nullptr)
      throw std::bad_alloc();
  }

  Buffer::Buffer(size_t size):
    _size(size),
    _capacity(size),
    _contents(nullptr)
  {
    if ((this->_contents =
         static_cast<Byte*>(::malloc(this->_capacity))) == nullptr)
      throw std::bad_alloc();
  }

  Buffer::Buffer(void const* data,
                 size_t size):
    _size(0),
    _capacity(0),
    _contents(nullptr)
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

  Buffer::Buffer(Buffer&& other):
    _size(0),
    _capacity(0),
    _contents(nullptr)
  {
    (*this) = std::move(other);
  }

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

  ELLE_SERIALIZE_CONSTRUCT_DEFINE(Buffer)
  {
    this->_contents = nullptr;
    this->_size = 0;
    this->_capacity = 0;
  }

  Buffer::~Buffer()
  {
    ::free(this->_contents);
  }

  void
  Buffer::capacity(size_t capacity)
  {
    if (capacity < ELLE_BUFFER_INITIAL_SIZE)
      capacity = ELLE_BUFFER_INITIAL_SIZE;
    void* tmp = ::realloc(this->_contents, capacity);
    if (tmp == nullptr)
      throw std::bad_alloc();
    this->_contents = static_cast<Byte*>(tmp);
    this->_capacity = capacity;
    this->_size = std::min(this->_size, capacity);
  }

  void Buffer::append(void const* data, size_t size)
  {
    ELLE_ASSERT(data != nullptr || size == 0);

    size_t old_size = this->_size;
    this->size(this->_size + size);
    /// XXX some implementations of memmove does not check for memory overlap
    memmove(this->_contents + old_size, data, size);
    // std::uninitialized_copy(
    //   static_cast<Byte const*>(data),
    //   static_cast<Byte const*>(data) + size,
    //   this->_contents + old_size
  }

  void
  Buffer::size(size_t size)
  {
    if (this->_capacity < size)
      {
        size_t next_size = Buffer::_next_size(size);
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

  OutputBufferArchive
  Buffer::writer()
  {
    return OutputBufferArchive(*this);
  }

  InputBufferArchive
  Buffer::reader() const
  {
    return InputBufferArchive(*this);
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

        for (size_t j = 0; j < (this->_size % space); j++)
          std::cout << std::nouppercase
                    << std::hex
                    << std::setw(2)
                    << (int)this->_contents[i * space + j];

        std::cout << std::endl;
      }
  }

  size_t
  Buffer::_next_size(size_t size)
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
    return WeakBuffer(_contents, _size) == WeakBuffer(ot._contents, ot._size);
  }

  bool
  Buffer::operator <(Buffer const& ot) const
  {
    return WeakBuffer(_contents, _size) < WeakBuffer(ot._contents, ot._size);
  }

  void
  Buffer::shrink_to_fit()
  {
    auto size = std::max(ELLE_BUFFER_INITIAL_SIZE, this->_size);
    if (size < this->_capacity)
    {
      void* tmp = ::realloc(_contents, size);
      if (tmp == nullptr)
        throw std::bad_alloc();
      this->_contents = static_cast<Byte*>(tmp);
      this->_capacity = size;
    }
  }


  std::ostream&
  operator <<(std::ostream& stream,
              Buffer const& buffer)
  {
    return stream << WeakBuffer(buffer);
  }

//
// ---------- WeakBuffer ------------------------------------------------------
//

  InputBufferArchive
  ConstWeakBuffer::reader() const
  {
    return InputBufferArchive(*this);
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

        for (size_t j = 0; j < (this->_size % space); j++)
          std::cout << std::nouppercase
                    << std::hex
                    << std::setw(2)
                    << (int)this->_contents[i * space + j];

        std::cout << std::endl;
      }
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
    if ((this->_size < other.size()) == false)
        return ::memcmp(this->_contents, other.contents(), this->_size) < 0;
    return true;
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

  std::ostream&
  operator <<(std::ostream& stream,
              WeakBuffer const& buffer)
  {
    static size_t const max_length = 20;

    // Display the string, depending on its length.
    if (buffer.size() == 0)
      stream << "empty";
    else if (buffer.size() <= max_length)
      stream << "0x" << format::hexadecimal::encode(buffer);
    else
      // Otherwise chop it and display the begining and the end only.
      stream << "0x"
             << format::hexadecimal::encode(
                 WeakBuffer{buffer.mutable_contents(), max_length / 2})
             << "..." << std::dec << buffer.size() << " bytes" << "..."
             << format::hexadecimal::encode(
                 WeakBuffer{
                   buffer.mutable_contents() + buffer.size() - max_length / 2,
                   max_length / 2});
    return stream;
  }

  /*------------------.
  | InputStreamBuffer |
  `------------------*/

  template <typename BufferType>
  InputStreamBuffer<BufferType>::InputStreamBuffer(BufferType const& buffer):
    _buffer(buffer),
    _read(false)
  {
    ELLE_DEBUG("create an InputStreamBuffer on a buffer of size %s",
               buffer.size());
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
    if (!this->_read)
    {
      this->_read = true;
      return WeakBuffer((char*)_buffer.contents(), _buffer.size());
    }
    else
      return WeakBuffer(nullptr, 0);
  }

  template <typename BufferType>
  void
  InputStreamBuffer<BufferType>::flush(Size size)
  {
    throw Exception("the buffer is in input mode");
  }

  ///////////////////////////////////////////////////////////////////////////

  InputBufferArchive::InputBufferArchive(ConstWeakBuffer const& buffer):
    serialize::InputBinaryArchive(
      *(_istream = new IOStream(new InputStreamBuffer<ConstWeakBuffer>(buffer)))
    )
  {}

  InputBufferArchive::InputBufferArchive(Buffer const& buffer):
    serialize::InputBinaryArchive(
      *(_istream = new IOStream(new InputStreamBuffer<Buffer>(buffer)))
    )
  {}

  InputBufferArchive::InputBufferArchive(InputBufferArchive&& other):
    serialize::InputBinaryArchive(*_istream),
    _istream(other._istream)
  {
    other._istream = nullptr;
  }

  InputBufferArchive::~InputBufferArchive()
  {
    delete _istream;
    _istream = nullptr;
  }

  /*-------------------.
  | OutputStreamBuffer |
  `-------------------*/

  OutputStreamBuffer::OutputStreamBuffer(Buffer& buffer):
    _old_size(buffer.size()),
    _buffer(buffer)
  {}

  WeakBuffer
  OutputStreamBuffer::write_buffer()
  {
    _buffer.size(_old_size + 512);
    ELLE_DEBUG("Grow stream buffer from %s to %s bytes", _old_size, _buffer.size());
    return WeakBuffer(
      (char*)_buffer.mutable_contents() + _old_size,
      512
      );
  }

  WeakBuffer
  OutputStreamBuffer::read_buffer()
  {
    throw Exception("the buffer is in output mode");
  }

  void
  OutputStreamBuffer::flush(Size size)
  {
    ELLE_DEBUG("Flush buffer stream size: %s", size);
    _old_size += size;
    _buffer.size(_old_size);
  }

  ///////////////////////////////////////////////////////////////////////////

  OutputBufferArchive::OutputBufferArchive(Buffer& buffer):
    serialize::OutputBinaryArchive(
      *(_ostream = new IOStream(new OutputStreamBuffer(buffer)))
    )
  {
    ELLE_DEBUG("create OutputBufferArchive %s stream %s", this, _ostream);
  }

  OutputBufferArchive::OutputBufferArchive(OutputBufferArchive&& other):
    serialize::OutputBinaryArchive(*_ostream),
    _ostream(other._ostream)
  {
    ELLE_DEBUG("move OutputBufferArchive %s stream %s", this, _ostream);
    other._ostream = nullptr;
  }

  OutputBufferArchive::~OutputBufferArchive()
  {
    ELLE_DEBUG("Delete OutputBufferArchive %s stream %s", this, _ostream);
    delete _ostream;
    _ostream = nullptr;
  }

}
