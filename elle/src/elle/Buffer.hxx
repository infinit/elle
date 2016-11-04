#ifndef  ELLE_BUFFER_HXX
# define ELLE_BUFFER_HXX

# include <elle/BadAlloc.hh>
# include <elle/assert.hh>
# include <elle/serialize/Serializer.hh>
# include <elle/serialize/BinaryArchive.hh>

# include <stdexcept>
# include <iosfwd>

namespace elle
{
  inline
  Byte*
  Buffer::mutable_contents() const
  {
    return this->_contents;
  }

  /*-------------.
  | Construction |
  `-------------*/
  template <typename T,
            typename std::enable_if<std::is_integral<T>::value, int>::type>
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

  class InputBufferArchive:
    public elle::serialize::InputBinaryArchive
  {
  private:
    std::istream* _istream;
    using Self = InputBufferArchive;

    explicit
    InputBufferArchive(std::istream* istream);

  public:
    explicit
    InputBufferArchive(ConstWeakBuffer const& buffer);
    explicit
    InputBufferArchive(Buffer const& buffer);
    InputBufferArchive(InputBufferArchive&& other);
    ~InputBufferArchive();

  private:
    InputBufferArchive(InputBufferArchive const& other);
    InputBufferArchive&
    operator =(InputBufferArchive const& other);
  };

  class OutputBufferArchive:
    public elle::serialize::OutputBinaryArchive
  {
  private:
    std::ostream* _ostream;
    using Self = OutputBufferArchive;

    explicit
    OutputBufferArchive(std::ostream* ostream);

  public:
    explicit
    OutputBufferArchive(Buffer& buffer);
    OutputBufferArchive(OutputBufferArchive&& other);
    ~OutputBufferArchive();

  private:
    OutputBufferArchive(OutputBufferArchive const& other);
    OutputBufferArchive&
    operator =(OutputBufferArchive const& other);
  };

}

ELLE_SERIALIZE_SPLIT(elle::Buffer)

ELLE_SERIALIZE_SPLIT_LOAD(elle::Buffer,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);
  uint64_t size;
  archive >> size;
  try
  {
    value.size(size);
  }
  catch (std::bad_alloc const& e)
  {
    throw elle::BadAlloc();
  }
  archive.LoadBinary(value._contents, size);
}

ELLE_SERIALIZE_SPLIT_SAVE(elle::Buffer,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);
  archive << static_cast<uint64_t>(value.size());
  archive.SaveBinary(value.contents(), value.size());
}


ELLE_SERIALIZE_SPLIT(elle::WeakBuffer)

ELLE_SERIALIZE_SPLIT_SAVE(elle::WeakBuffer,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);
  archive << static_cast<uint64_t>(value.size());
  archive.SaveBinary(value.contents(), value.size());
}

ELLE_SERIALIZE_SPLIT_LOAD(elle::WeakBuffer,,,)
{
  elle::unreachable();
}

#endif
