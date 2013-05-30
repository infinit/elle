#ifndef  ELLE_BUFFER_HXX
# define ELLE_BUFFER_HXX

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

  inline
  WeakBuffer::WeakBuffer(void* data, size_t size):
    _size(size),
    _contents(static_cast<Byte*>(data))
  {}

  inline
  WeakBuffer::WeakBuffer(Buffer const& buffer):
    _size(buffer.size()),
    _contents(buffer.mutable_contents())
  {}

  inline
  WeakBuffer::WeakBuffer(WeakBuffer const& other):
    _size(other._size),
    _contents(other._contents)
  {}

  inline
  WeakBuffer::WeakBuffer(WeakBuffer&& other):
    _size(other._size),
    _contents(other._contents)
  {
    other._contents = nullptr;
    other._size = 0;
  }

  inline
  Byte*
  WeakBuffer::mutable_contents() const
  {
    return this->_contents;
  }

  class InputBufferArchive:
    public elle::serialize::InputBinaryArchive
  {
  private:
    std::istream* _istream;

  public:
    explicit
    InputBufferArchive(WeakBuffer const& buffer);
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
  if (sizeof(uint64_t) > sizeof(size_t))
    {
      if (size >= static_cast<size_t>(-1))
        throw std::runtime_error("Cannot receive a buffer that large");
    }
  value.size(size);
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
