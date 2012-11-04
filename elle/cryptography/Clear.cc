#include <elle/cryptography/Clear.hh>
#include <elle/cryptography/Plain.hh>

namespace elle
{
  namespace cryptography
  {
    /*-------------.
    | Construction |
    `-------------*/

    Clear::Clear(elle::Natural64 const size):
      _buffer(size)
    {
    }

    Clear::Clear(Clear const& other):
      _buffer(other._buffer.contents(), other._buffer.size())
    {
    }

    Clear::Clear(Clear&& other):
      _buffer(std::move(other._buffer))
    {
    }

    /*----------.
    | Operators |
    `----------*/

    Boolean
    Clear::operator ==(Clear const& other) const
    {
      if (this == &other)
        return (true);

      return (this->_buffer == other._buffer);
    }

    Boolean
    Clear::operator <(Clear const& other) const
    {
      if (this == &other)
        return (false);

      return (this->_buffer < other._buffer);
    }

    Boolean
    Clear::operator <=(Clear const& other) const
    {
      if (this == &other)
        return (true);

      return (this->_buffer <= other._buffer);
    }

    Boolean
    Clear::operator ==(Plain const& other) const
    {
      return (WeakBuffer(this->_buffer) == other.buffer());
    }

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status
    Clear::Dump(elle::Natural32           margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Clear] " << this << std::endl;

      this->_buffer.dump(margin + 2);

      return elle::Status::Ok;
    }
  }
}
