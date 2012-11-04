#include <elle/cryptography/Digest.hh>

namespace elle
{
  namespace cryptography
  {
    /*-------------.
    | Construction |
    `-------------*/

    Digest::Digest(elle::Natural64 const size):
      _buffer(size)
    {
    }

    Digest::Digest(Digest const& other):
      _buffer(other._buffer.contents(), other._buffer.size())
    {
    }

    Digest::Digest(Digest&& other):
      _buffer(std::move(other._buffer))
    {
    }

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status
    Digest::Dump(elle::Natural32           margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Digest] " << this << std::endl;

      this->_buffer.dump(margin + 2);

      return elle::Status::Ok;
    }

    /*----------.
    | Operators |
    `----------*/

    Boolean
    Digest::operator ==(Digest const& other) const
    {
      if (this == &other)
        return (true);

      return (this->_buffer == other._buffer);
    }

    Boolean
    Digest::operator <(Digest const& other) const
    {
      if (this == &other)
        return (false);

      return (this->_buffer < other._buffer);
    }

    Boolean
    Digest::operator <=(Digest const& other) const
    {
      if (this == &other)
        return (true);

      return (this->_buffer <= other._buffer);
    }

  }
}
