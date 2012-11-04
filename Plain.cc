#include <elle/cryptography/Plain.hh>
#include <elle/cryptography/Clear.hh>

namespace elle
{
  namespace cryptography
  {

    /*-------------.
    | Construction |
    `-------------*/

    Plain::Plain(WeakBuffer const& buffer):
      _buffer(buffer)
    {
    }

    /*----------.
    | Operators |
    `----------*/

    Boolean
    Plain::operator ==(Plain const& other) const
    {
      return (this->_buffer == other._buffer);
    }

    Boolean
    Plain::operator ==(Clear const& other) const
    {
      return (this->_buffer == WeakBuffer(other.buffer()));
    }

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status
    Plain::Dump(elle::Natural32           margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Plain] " << this << std::endl;

      this->_buffer.dump(margin + 2);

      return elle::Status::Ok;
    }

  }
}
