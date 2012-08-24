#include <elle/log.hh>

#include <protocol/Channel.hh>
#include <protocol/ChanneledStream.hh>

ELLE_LOG_COMPONENT("infinit.protocol.Channel");

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
      ELLE_DEBUG_SCOPE("%s: open %s", _backend, *this);
      _backend._channels[_id] = this;
    }

    Channel::Channel(ChanneledStream& backend, int id)
      : Super(backend.scheduler())
      , _backend(backend)
      , _id(id)
    {
      ELLE_DEBUG_SCOPE("%s: open %s", _backend, *this);
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
      assert(_backend._channels.find(_id) != _backend._channels.end());
      _backend._channels[_id] = this;
    }

    Channel::~Channel()
    {
      assert(_packets.empty());
      if (_id != 0)
        {
          ELLE_DEBUG_SCOPE("%s: close %s", _backend, *this);
          assert(_backend._channels.find(_id) != _backend._channels.end());
          _backend._channels.erase(_id);
        }
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
    Channel::_write(Packet& packet)
    {
      _backend._write(packet, _id);
    }
  }
}
