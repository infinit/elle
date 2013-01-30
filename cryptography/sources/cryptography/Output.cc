#include <cryptography/Output.hh>

namespace infinit
{
  namespace cryptography
  {
    /*-------------.
    | Construction |
    `-------------*/

    Output::Output(elle::Natural64 const size):
      _buffer(size)
    {
    }

    Output::Output(Output const& other):
      _buffer(other._buffer.contents(), other._buffer.size())
    {
    }

    Output::Output(Output&& other):
      _buffer(std::move(other._buffer))
    {
    }

    ELLE_SERIALIZE_CONSTRUCT_DEFINE(Output,
                                    _buffer)
    {
    }

    /*----------.
    | Operators |
    `----------*/

    elle::Boolean
    Output::operator ==(Output const& other) const
    {
      if (this == &other)
        return (true);

      return (this->_buffer == other._buffer);
    }

    elle::Boolean
    Output::operator <(Output const& other) const
    {
      if (this == &other)
        return (false);

      return (this->_buffer < other._buffer);
    }

    elle::Boolean
    Output::operator <=(Output const& other) const
    {
      if (this == &other)
        return (true);

      return (this->_buffer <= other._buffer);
    }

    elle::Boolean
    Output::operator ==(Input const& other) const
    {
      return (elle::WeakBuffer(this->_buffer) == other.buffer());
    }

    elle::Boolean
    Output::operator <(Input const& other) const
    {
      return (elle::WeakBuffer(this->_buffer) < other.buffer());
    }

    elle::Boolean
    Output::operator <=(Input const& other) const
    {
      return (elle::WeakBuffer(this->_buffer) <= other.buffer());
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Output::print(std::ostream& stream) const
    {
      stream << this->_buffer;
    }
  }
}
