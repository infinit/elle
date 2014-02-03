#include <elle/OStream.hh>
#include <elle/log.hh>

ELLE_LOG_COMPONENT("elle.OStream");

namespace elle
{
  /*-------------.
  | Construction |
  `-------------*/

  OStream::OStream(std::unique_ptr<Backend> backend):
    _backend(std::move(backend)),
    _buffer()
  {
    this->_buffer.capacity(this->_backend->size());
    ELLE_DEBUG("%s: initialize internal buffer with size %s",
               *this, this->_buffer.capacity());
  }

  OStream::~OStream() noexcept(false)
  {
    this->flush();
  }

  /*--------.
  | Writing |
  `--------*/
  void
  OStream::write(Byte data)
  {
    this->write(elle::ConstWeakBuffer(&data, 1));
  }

  void
  OStream::write(elle::ConstWeakBuffer const& data)
  {
    ELLE_DEBUG_SCOPE("%s: bufferize %s bytes", *this, data.size());
    ELLE_DUMP("%s: data: %s", *this, data);
    unsigned written = 0;
    if (this->_buffer.size() > 0)
    {
      auto pos = this->_buffer.size();
      written = std::min(data.size(), this->_buffer.capacity() - pos);
      memcpy(this->_buffer.mutable_contents() + pos,
             data.contents(), written);
      this->_buffer.size(this->_buffer.size() + written);
      if (this->_buffer.size() == this->_buffer.capacity())
      {
        ELLE_TRACE("%s: send buffer of %s bytes", *this, this->_buffer.size())
        {
          ELLE_DUMP("%s: data: %s", *this, this->_buffer);
          this->_backend->write(this->_buffer);
        }
        this->_buffer.size(0);
      }
      else
        ELLE_DEBUG("%s: keep %s bytes", *this, this->_buffer.size());
    }
    while (written < data.size())
    {
      auto left = data.size() - written;
      if (left < this->_buffer.capacity())
      {
        memcpy(this->_buffer.mutable_contents(),
               data.contents() + written, left);
        written += left;
        this->_buffer.size(left);
        ELLE_DEBUG("%s: keep last %s bytes", *this, left);
      }
      else
      {
        auto size = this->_backend->size(left);
        ELLE_ASSERT_GT(size, 0);
        elle::ConstWeakBuffer chunk(data.contents() + written, size);
        ELLE_TRACE("%s: send buffer of %s bytes", *this, chunk.size())
        {
          ELLE_DUMP("%s: data: %s", *this, chunk);
          this->_backend->write(chunk);
        }
        written += size;
      }
    }
  }

  void
  OStream::flush()
  {
    if (this->_buffer.size() > 0)
      this->_backend->write(this->_buffer);
  }

  /*--------.
  | Backend |
  `--------*/

  OStream::Size
  OStream::Backend::size(Size size)
  {
    return std::min(size, this->size());
  }
}
