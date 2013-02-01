#include <nucleus/neutron/Data.hh>
#include <nucleus/proton/Nest.hh>
#include <nucleus/proton/Ambit.hh>
#include <nucleus/proton/Contents.hh>
#include <nucleus/proton/Limits.hh>

#include <elle/assert.hh>
#include <elle/finally.hh>
#include <elle/log.hh>

ELLE_LOG_COMPONENT("infinit.nucleus.proton.Data");

namespace nucleus
{
  namespace neutron
  {
    /*----------.
    | Constants |
    `----------*/

    proton::Nature const Data::Constants::seam =
      proton::Nature::data_seam;
    proton::Nature const Data::Constants::quill =
      proton::Nature::data_quill;
    proton::Nature const Data::Constants::value =
      proton::Nature::data_value;
    proton::Nature const Data::Constants::nature =
      Data::Constants::value;

    /*---------------.
    | Static Methods |
    `---------------*/

    void
    Data::transfer_right(Data& left,
                         Data& right,
                         proton::Extent const size)
    {
      ELLE_TRACE_FUNCTION(left, right, size);

      // XXX[Data() should not be used]
      static proton::Footprint const initial =
        elle::serialize::footprint<Data>();
      proton::Footprint footprint(initial);

      // Compute the amount of content to keep i.e not in terms of
      // footprint: the total footprint to maintain minus the initial
      // footprint.
      Size size_left = size - footprint;
      Size size_right = left._buffer.size() - size_left;

      // Keep the old size of the right data.
      Size _size = right._buffer.size();

      // Extend the size of the right buffer.
      right._buffer.size(right._buffer.size() + size_right);

      // Move the existing data in the right value so as to welcome the
      // new content.
      if (_size != 0)
        ::memmove(right._buffer.mutable_contents() + size_right,
                  right._buffer.contents(),
                  _size);

      ::memcpy(right._buffer.mutable_contents(),
               left._buffer.contents() + size_left,
               size_right);

      // Finally, shrink the left buffer.
      left._buffer.size(size_left);

      // Update the left and right footprints.
      ELLE_ASSERT(left.footprint() > size_right);
      left.footprint(left.footprint() - size_right);
      right.footprint(right.footprint() + size_right);
    }

    void
    Data::transfer_left(Data& left,
                        Data& right,
                        proton::Extent const size)
    {
      ELLE_TRACE_FUNCTION(left, right, size);

      static proton::Footprint const initial =
        elle::serialize::footprint<Data>();
      proton::Footprint footprint(initial);

      // Compute the amount of content to keep i.e not in terms of
      // footprint: the total footprint to maintain minus the initial
      // footprint.
      Size size_right = size - footprint;
      Size size_left = right._buffer.size() - size_right;

      // Keep a copy of the current size of the left buffer.
      Size _size = left._buffer.size();

      // Extend the size of the left buffer.
      left._buffer.size(left._buffer.size() + size_left);

      // Append the content to the left buffer.
      ::memcpy(left._buffer.mutable_contents() + _size,
               right._buffer.contents(),
               size_left);

      // Shift the right content.
      ::memmove(right._buffer.mutable_contents(),
                right._buffer.contents() + size_left,
                size_right);

      // Update the left and right footprints.
      left.footprint(left.footprint() + size_left);
      ELLE_ASSERT(right.footprint() > size_left);
      right.footprint(right.footprint() - size_left);
    }

    /*-------------.
    | Construction |
    `-------------*/

    Data::Data():
      Value::Value(),

      _offset(0)
    {
      static proton::Footprint const initial =
        elle::serialize::footprint(*this);

      this->footprint(initial);
    }

    Data::Data(Data const& other):
      Value::Value(other),

      _offset(other._offset),
      _buffer(other._buffer.contents(),
              other._buffer.size())
    {
    }

    ELLE_SERIALIZE_CONSTRUCT_DEFINE(Data, proton::Value)
    {
    }

    /*--------.
    | Methods |
    `--------*/

    void
    Data::write(Offset const& offset,
                elle::WeakBuffer const& buffer)
    {
      ELLE_TRACE_METHOD(offset, buffer);

      // Compute the offset reached by the written buffer.
      Offset _offset = offset + buffer.size();

      // Compute the extension of the size, if any.
      Size _size = _offset > this->_buffer.size() ?
        _offset - this->_buffer.size() : 0;

      // Possibly extend the data buffer.
      if (_size > 0)
        this->_buffer.size(this->_buffer.size() + _size);

      // Write the data.
      ::memcpy(this->_buffer.mutable_contents() + offset,
               buffer.contents(),
               buffer.size());

      // Update the footprint and state.
      this->footprint(this->footprint() + _size);
      this->state(proton::State::dirty);
    }

    elle::Buffer
    Data::read(Offset const& offset,
               Size const& size) const
    {
      ELLE_TRACE_METHOD(offset, size);

      elle::Buffer buffer;

      this->read(offset, size, buffer);

      return (buffer);
    }

    void
    Data::read(Offset const& offset,
               Size const& size,
               elle::Buffer& buffer) const
    {
      ELLE_TRACE_METHOD(offset, size, buffer);

      // Compute the size of the data read between what is available
      // and what is requested.
      Size _size = (this->_buffer.size() - offset) < size ?
        (this->_buffer.size() - offset) : size;

      // Keep the current size of the buffer at which the data will
      // be copied.
      Offset _offset = static_cast<Offset>(buffer.size());

      // Extend the buffer so as to hold the additional content.
      buffer.size(buffer.size() + _size);

      // Copy the data to the output buffer.
      ::memcpy(buffer.mutable_contents() + _offset,
               this->_buffer.contents() + offset,
               _size);
    }

    void
    Data::adjust(Size const& size)
    {
      ELLE_TRACE_METHOD(size);

      // Depending on the fact that the data is being extended or shrunk.
      if (size > this->_buffer.size())
        {
          // Compute the difference between the old and new size.
          elle::Natural64 _size = size - this->_buffer.size();

          // Adjust the size of the buffer.
          this->_buffer.size(size);

          // XXX[fill with zeros]

          // Update the footprint and state.
          this->footprint(this->footprint() + _size);
        }
      else
        {
          // Compute the difference between the old and new size.
          elle::Natural64 _size = this->_buffer.size() - size;

          // Adjust the size of the buffer.
          this->_buffer.size(size);

          // Update the footprint and state.
          ELLE_ASSERT(this->footprint() >= _size);
          this->footprint(this->footprint() - _size);
        }

      this->state(proton::State::dirty);
    }

    Size
    Data::size() const
    {
      return (static_cast<Size>(this->_buffer.size()));
    }

    /*---------.
    | Dumpable |
    `---------*/

    elle::Status
    Data::Dump(elle::Natural32              margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Data] " << std::hex << this << std::endl;

      if (Value::Dump(margin + 2) == elle::Status::Error)
        throw Exception("unable to dump the value");

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Offset] " << std::dec << this->_offset << std::endl;

      std::cout << alignment << elle::io::Dumpable::Shift
                << "[Buffer] " << this->_buffer << std::endl;

      return elle::Status::Ok;
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Data::print(std::ostream& stream) const
    {
      stream << "data("
             << this->_offset
             << ", "
             << this->_buffer.size()
             << ")";
    }

    /*------.
    | Value |
    `------*/

    elle::Boolean
    Data::empty() const
    {
      return (this->_buffer.size() == 0);
    }

    Offset
    Data::mayor() const
    {
      ELLE_ASSERT(this->_buffer.size() != 0);

      // Return the highest offset i.e the offset of the last byte
      // in the buffer.
      return (this->_offset + this->_buffer.size() - 1);
    }

    proton::Capacity
    Data::capacity() const
    {
      return (static_cast<proton::Capacity>(this->_buffer.size()));
    }

    proton::Handle
    Data::split()
    {
      ELLE_TRACE_METHOD("");

      // Allocate a new right data.
      proton::Contents* contents =
        new proton::Contents{this->nest().network(),
                             this->nest().agent_K(),
                             new Data};
      // XXX[change to extent * contention though contention = 100%]
      proton::Handle orphan{this->nest().attach(contents)};
      proton::Ambit<Data> right{this->nest(), orphan};

      // Load the new right data.
      right.load();

      // Export part of the buffer from the current data into the new data.
      Data::transfer_right(*this,
                           right(),
                           this->nest().limits().extent() *
                           this->nest().limits().contention());

      // Update the offset.
      right()._offset = this->_offset + this->capacity();

      // Set both values' state as dirty.
      this->state(proton::State::dirty);
      right().state(proton::State::dirty);

      // Unload the new right data.
      right.unload();

      return (orphan);
    }

    void
    Data::merge(proton::Handle& other)
    {
      ELLE_TRACE_METHOD(other);

      proton::Ambit<Data> data(this->nest(), other);

      // Load the other data.
      data.load();

      // Check which value has the lowest keys.
      if (data().mayor() < this->mayor())
        {
          // In this case, export the buffer from the given data
          // into the current's since the offsets happen to be lower.
          //
          // Note that the footprint-based number of bytes to keep in
          // the left data is zero i.e transfer all bytes.
          Data::transfer_right(data(), *this, 0);

          // Update the offset since this data received the low offset
          // bytes from its neighbour.
          this->_offset = data()._offset;
        }
      else
        {
          // Otherwise, import the higher offsets from the given data
          // into the current's.
          //
          // Note that the footprint-based number of bytes to keep in
          // the right data is zero i.e transfer all bytes.
          Data::transfer_left(*this, data(), 0);

          // In this case the offset has not changed since the high offsets
          // bytes have been transferred.
        }

      // Set both values' state as dirty.
      this->state(proton::State::dirty);
      data().state(proton::State::dirty);

      // Unload the given data.
      data.unload();
    }
  }
}
