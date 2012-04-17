
#include <stdexcept>

#include <elle/utility/Buffer.hh>

using namespace elle::utility;


Buffer::Buffer()
  : _contents(nullptr)
  , _size(0)
  , _buffer_size(0)
{}

Buffer::Buffer(size_t size)
  : _contents(nullptr)
  , _size(size)
  , _buffer_size(Buffer::_NextSize(size))
{
  this->_contents = static_cast<elle::Byte*>(::malloc(_buffer_size));
  if (this->_contents == nullptr)
    throw std::bad_alloc();
}

Buffer::Buffer(Buffer&& other) :
  : _contents(other._contents)
  , _size(other._size)
  , _buffer_size(other._buffer_size)
{
#ifdef DEBUG
  other._contents = nullptr;
  other._size = 0;
  other._buffer_size = 0;
#endif
}

Buffer::Buffer& operator =(Buffer&& other);

Buffer::~Buffer()
{
  ::free(this->_contents);
}

void Buffer::Append(void const* data, size_t size)
{
  elle::Byte* ptr = &this->_contents[this->_size];
  size_t new_size = this->_size + size;
  if (new_size > this->_buffer_size)
    this->Size(new_size);
  else
    this->_size = new_size;
  std::unitialized_copy(data, size, ptr);
}

void Buffer::Size(size_t size)
{
  if (this->_buffer_size < size)
    {
      size_t next_size = Buffer::_NextSize(size);
      ContentPtr::element_type* tmp = ::realloc(_contents, next_size);
      if (tmp == nullptr)
        throw std::bad_alloc();
      this->_contents = tmp;
      this->_buffer_size = next_size;
    }
  this->_size = size;
}

static size_t _NextSize(size_t size)
{
  if (size < 32)
    return 32;
  if (size < 4096)
    return size *= 2;
  return (size + size  / 2);
}
