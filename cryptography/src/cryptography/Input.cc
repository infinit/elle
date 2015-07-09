#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
# include <cryptography/Input.hh>
# include <cryptography/Output.hh>

namespace infinit
{
  namespace cryptography
  {
    /*-------------.
    | Construction |
    `-------------*/

    Input::Input(elle::ConstWeakBuffer const& buffer):
      _buffer(buffer)
    {}

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
      return (this->_buffer == elle::ConstWeakBuffer(other.buffer()));
    }

    elle::Boolean
    Input::operator <(Output const& other) const
    {
      return (this->_buffer < elle::ConstWeakBuffer(other.buffer()));
    }

    elle::Boolean
    Input::operator <=(Output const& other) const
    {
      return (this->_buffer <= elle::ConstWeakBuffer(other.buffer()));
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

#endif
