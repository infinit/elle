#ifndef  ELLE_BUFFER_HXX
# define ELLE_BUFFER_HXX

# include <elle/BadAlloc.hh>
# include <elle/assert.hh>

# include <stdexcept>
# include <iosfwd>

namespace elle
{
  inline
  void
  detail::MallocDeleter::operator ()(void* data)
  {
    ::free(data);
  }

  /*-------------.
  | Construction |
  `-------------*/

  template <typename T,
            std::enable_if_t<std::is_integral<T>::value, int>>
  Buffer::Buffer(T size)
    : _size(static_cast<Size>(size))
    , _capacity(size)
    , _contents(nullptr)
  {
    if ((this->_contents =
         static_cast<Byte*>(::malloc(this->_capacity))) == nullptr)
      throw std::bad_alloc();
  }

  inline
  ConstWeakBuffer::ConstWeakBuffer():
    _size(0),
    _contents(nullptr)
  {}

  inline
  ConstWeakBuffer::ConstWeakBuffer(const void* data, Size size):
    _size(size),
    _contents(static_cast<Byte*>(const_cast<void*>(data)))
  {}

  inline
  ConstWeakBuffer::ConstWeakBuffer(std::string const& data):
    ConstWeakBuffer(data.c_str(), data.size())
  {}

  inline
  ConstWeakBuffer::ConstWeakBuffer(char const* data):
    ConstWeakBuffer(data, strlen(data))
  {}

  inline
  ConstWeakBuffer::ConstWeakBuffer(Buffer const& buffer):
    ConstWeakBuffer(buffer.mutable_contents(), buffer.size())
  {}

  inline
  ConstWeakBuffer::ConstWeakBuffer(ConstWeakBuffer const& other):
    ConstWeakBuffer(other._contents, other._size)
  {}

  inline
  ConstWeakBuffer::ConstWeakBuffer(ConstWeakBuffer&& other):
    ConstWeakBuffer(other)
  {
    other._contents = nullptr;
    other._size = 0;
  }

  inline
  WeakBuffer::WeakBuffer():
    ConstWeakBuffer()
  {}

  inline
  WeakBuffer::WeakBuffer(void* data, Size size):
    ConstWeakBuffer(data, size)
  {}

  inline
  WeakBuffer::WeakBuffer(Buffer const& buffer):
    ConstWeakBuffer(buffer)
  {}

  inline
  WeakBuffer::WeakBuffer(WeakBuffer const& other):
    ConstWeakBuffer(other)
  {}

  inline
  WeakBuffer::WeakBuffer(WeakBuffer&& other):
    ConstWeakBuffer(std::move(other))
  {}

  inline
  Byte*
  WeakBuffer::mutable_contents() const
  {
    return const_cast<Byte*>(this->contents());
  }

  inline
  Byte*
  Buffer::mutable_contents() const
  {
    return this->_contents;
  }
}

#endif
