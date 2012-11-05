#include <elle/cryptography/Input.hh>

namespace elle
{
  namespace cryptography
  {
    /*-------------.
    | Construction |
    `-------------*/

    Input::Input(WeakBuffer const& buffer):
      _buffer(buffer)
    {
    }

    /*----------.
    | Operators |
    `----------*/

    Boolean
    Input::operator ==(Input const& other) const
    {
      if (this == &other)
        return (true);

      return (this->_buffer == other._buffer);
    }

    Boolean
    Input::operator <(Input const& other) const
    {
      if (this == &other)
        return (false);

      return (this->_buffer < other._buffer);
    }

    Boolean
    Input::operator <=(Input const& other) const
    {
      if (this == &other)
        return (true);

      return (this->_buffer <= other._buffer);
    }

    Boolean
    Input::operator ==(Output const& other) const
    {
      return (this->_buffer == WeakBuffer(other.buffer()));
    }

    Boolean
    Input::operator <(Output const& other) const
    {
      return (this->_buffer < WeakBuffer(other.buffer()));
    }

    Boolean
    Input::operator <=(Output const& other) const
    {
      return (this->_buffer <= WeakBuffer(other.buffer()));
    }

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status
    Input::Dump(elle::Natural32           margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Input] " << this << std::endl;

      this->_buffer.dump(margin + 2);

      return elle::Status::Ok;
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Input::print(std::ostream& stream) const
    {
      stream << this->_buffer;
    }
  }
}
