#include <cryptography/Input.hh>

#include <elle/format/hexadecimal.hh>

namespace infinit
{
  namespace cryptography
  {
    /*-------------.
    | Construction |
    `-------------*/

    Input::Input(elle::WeakBuffer const& buffer):
      _buffer(buffer)
    {
    }

    /*----------.
    | Operators |
    `----------*/

    elle::Boolean
    Input::operator ==(Input const& other) const
    {
      if (this == &other)
        return (true);

      return (this->_buffer == other._buffer);
    }

    elle::Boolean
    Input::operator <(Input const& other) const
    {
      if (this == &other)
        return (false);

      return (this->_buffer < other._buffer);
    }

    elle::Boolean
    Input::operator <=(Input const& other) const
    {
      if (this == &other)
        return (true);

      return (this->_buffer <= other._buffer);
    }

    elle::Boolean
    Input::operator ==(Output const& other) const
    {
      return (this->_buffer == elle::WeakBuffer(other.buffer()));
    }

    elle::Boolean
    Input::operator <(Output const& other) const
    {
      return (this->_buffer < elle::WeakBuffer(other.buffer()));
    }

    elle::Boolean
    Input::operator <=(Output const& other) const
    {
      return (this->_buffer <= elle::WeakBuffer(other.buffer()));
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
      stream <<
        elle::format::hexadecimal::encode(
          reinterpret_cast<const char*>(this->_buffer.contents()),
          this->_buffer.size());
    }
  }
}
