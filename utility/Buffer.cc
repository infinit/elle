
#include <iomanip>
#include <memory>
#include <stdexcept>

#include <elle/serialize/BufferArchive.hh>

#include <elle/utility/BufferStream.hh>
#include <elle/utility/Buffer.hh>

namespace elle
{
  namespace utility
  {

    void detail::MallocDeleter::operator ()(void* data)
    {
      ::free(data);
    }

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

    Buffer::Buffer(ContentPair&& pair)
      : _contents(pair.first.release())
      , _size(pair.second)
      , _buffer_size(pair.second)
    {}

    Buffer::Buffer(elle::Byte const* data, size_t size)
      : _contents(nullptr)
      , _size(0)
      , _buffer_size(0)
    {
      this->Append(data, size);
    }

    Buffer::Buffer(Buffer&& other)
      : _contents(other._contents)
      , _size(other._size)
      , _buffer_size(other._buffer_size)
    {
      other._contents = nullptr;
#ifdef DEBUG
      other._size = 0;
      other._buffer_size = 0;
#endif
    }

    Buffer::~Buffer()
    {
      ::free(this->_contents);
    }

    void Buffer::Append(void const* data, size_t size)
    {
      assert(data != nullptr || size == 0);

      size_t old_size = this->_size;
      this->Size(this->_size + size);
      // std::cerr << "COPY: " << (void*)data << " (" << size << ") to " << (void*)(this->_contents + old_size) << std::endl;
      memmove(this->_contents + old_size, data, size);
      // std::uninitialized_copy(
      //   static_cast<elle::Byte const*>(data),
      //   static_cast<elle::Byte const*>(data) + size,
      //   this->_contents + old_size
    }

    void Buffer::Size(size_t size)
    {
      if (this->_buffer_size < size)
        {
          size_t next_size = Buffer::_NextSize(size);
          void* tmp = ::realloc(_contents, next_size);
          if (tmp == nullptr)
            throw std::bad_alloc();
          this->_contents = static_cast<unsigned char*>(tmp);
          this->_buffer_size = next_size;
        }
      this->_size = size;
    }

    Buffer::ContentPair Buffer::Release()
    {
      ContentPair res(ContentPtr(this->_contents), this->_size);
      this->_contents = nullptr;
      this->_size = 0;
      this->_buffer_size = 0;
      return res;
    }

    elle::serialize::OutputBufferArchive Buffer::Writer()
    {
      elle::utility::OutputBufferStream out(*this);
      return elle::serialize::OutputBufferArchive(out);
    }

    elle::serialize::InputBufferArchive  Buffer::Reader() const
    {
      elle::utility::InputBufferStream in(*this);
      return elle::serialize::InputBufferArchive(in);
    }

    elle::Status Buffer::Dump(const Natural32 margin) const
    {
      Natural32         space = 78 - margin - Dumpable::Shift.length();
      String            alignment(margin, ' ');
      Natural32         i;
      Natural32         j;

      std::cout << alignment
                << "[Buffer] "
                << "address(" << static_cast<Void*>(this->_contents) << ") "
                << "size(" << std::dec << this->_size << ") "
                << "capacity(" << std::dec << this->_buffer_size << ")"
                << std::endl;

      // since a byte is represented by two characters.
      space = space / 2;

      // set the fill to '0'.
      std::cout.fill('0');

      // display the region.
      for (i = 0; i < (this->_size / space); i++)
        {
          std::cout << alignment << Dumpable::Shift;

          for (j = 0; j < space; j++)
            std::cout << std::nouppercase
                      << std::hex
                      << std::setw(2)
                      << (int)this->_contents[i * space + j];

          std::cout << std::endl;
        }

      if ((this->_size % space) != 0)
        {
          std::cout << alignment << Dumpable::Shift;

          for (size_t j = 0; j < (this->_size % space); j++)
            std::cout << std::nouppercase
                      << std::hex
                      << std::setw(2)
                      << (int)this->_contents[i * space + j];

          std::cout << std::endl;
        }

      return Status::Ok;
    }

    size_t Buffer::_NextSize(size_t size)
    {
      if (size < 32)
        return 32;
      if (size < 4096)
        return size *= 2;
      return (size + size  / 2);
    }

    elle::serialize::InputBufferArchive  WeakBuffer::Reader() const
    {
      elle::utility::InputBufferStream in(*this);
      return elle::serialize::InputBufferArchive(in);
    }

  }
}
