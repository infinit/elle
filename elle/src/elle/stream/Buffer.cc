#include <elle/assert.hh>
#include <elle/stream/Buffer.hh>

namespace elle
{
  OStream
  ostream(elle::Buffer& buffer)
  {
    return OStream(elle::make_unique<stream::Buffer>(buffer));
  }

  namespace stream
  {
    /*-------------.
    | Construction |
    `-------------*/

    Buffer::Buffer(elle::Buffer& buffer):
      _buffer(buffer)
    {}

    /*--------.
    | Writing |
    `--------*/

    void
    Buffer::write(elle::ConstWeakBuffer const& data)
    {
      auto capacity = this->_buffer.capacity();
      while (capacity - this->_buffer.size() < data.size())
        capacity *= 2;
      this->_buffer.capacity(capacity);
      ELLE_ASSERT_GTE(this->_buffer.capacity() - this->_buffer.size(),
                      data.size());
      memcpy(this->_buffer.mutable_contents() + this->_buffer.size(),
             data.contents(), data.size());
      this->_buffer.size(this->_buffer.size() + data.size());
    }

    Buffer::Size
    Buffer::size()
    {
      return 1 << 12;
    }

    Buffer::Size
    Buffer::size(Size s)
    {
      return s;
    }
  }
}
