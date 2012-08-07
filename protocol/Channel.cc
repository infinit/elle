#include <protocol/Channel.hh>
#include <protocol/ChanneledStream.hh>

namespace infinit
{
  namespace protocol
  {
    /*-------------.
    | Construction |
    `-------------*/

    // Factor those two by delegating the constructor.
    Channel::Channel(ChanneledStream& backend)
      : Super(backend.scheduler())
      , _backend(backend)
      , _id(backend._id_generate()) // FIXME: overflow
    {
      _backend._channels[_id] = this;
    }

    Channel::Channel(ChanneledStream& backend, int id)
      : Super(backend.scheduler())
      , _backend(backend)
      , _id(id)
    {
      _backend._channels[_id] = this;
    }

    Channel::Channel(Channel&& source)
      : Super(source.scheduler())
      , _backend(source._backend)
      , _id(source._id)
      , _packets(std::move(source._packets))
      , _available(std::move(source._available))
    {
      source._id = 0;
    }

    Channel::~Channel()
    {
      assert(_packets.empty());
      if (_id != 0)
        _backend._channels.erase(_id);
    }

    /*---------.
    | Printing |
    `---------*/

    void
    Channel::print(std::ostream& s) const
    {
      s << "channel " << _id;
    }

    /*-----------.
    | Properties |
    `-----------*/

    int
    Channel::id() const
    {
      return this->_id;
    }

    /*----------.
    | Receiving |
    `----------*/

    Packet
    Channel::read()
    {
      return _backend._read(this);
    }

    /*--------.
    | Sending |
    `--------*/

    void
    Channel::write(Packet& packet)
    {
      _backend._write(packet, _id);
    }
  }
}
