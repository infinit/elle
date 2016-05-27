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

    Channel::Channel(ChanneledStream& backend, int id)
      : Super(backend.scheduler())
      , _backend(backend)
      , _id(id)
    {
      ELLE_DEBUG_SCOPE("%s: open %s", this->_backend, *this);
      this->_backend._channels[this->_id] = this;
    }

    // Factor those two by delegating the constructor.
    Channel::Channel(ChanneledStream& backend)
      : Channel(backend, backend._id_generate())
    {
    }

    Channel::Channel(Channel&& source)
      : Super(source.scheduler())
      , _backend(source._backend)
      , _id(source._id)
      , _packets(std::move(source._packets))
      , _available(std::move(source._available))
    {
      source._id = 0;
      ELLE_ASSERT_NEQ(this->_backend._channels.find(this->_id),
                      this->_backend._channels.end());
      this->_backend._channels[this->_id] = this;
    }

    Channel::~Channel()
    {
      if (this->_id != 0)
        {
          ELLE_DEBUG_SCOPE("close channel %s", this->_id);
          if (!this->_packets.empty())
            ELLE_TRACE("closing channel %s with %s packets in queue.", this->_id,
                      this->_packets.size());
          if (!this->_available.waiters().empty())
            ELLE_TRACE("closing channel %s with %s waiters in line.", this->_id,
                      this->_available.waiters().size());
          ELLE_ASSERT_NEQ(this->_backend._channels.find(this->_id),
                          this->_backend._channels.end());
          this->_backend._channels.erase(this->_id);
        }
    }

    /*---------.
    | Printing |
    `---------*/

    void
    Channel::print(std::ostream& s) const
    {
      s << "channel " << this->_id;
    }

    /*----------.
    | Receiving |
    `----------*/

    elle::Buffer
    Channel::read()
    {
      return this->_backend._read(this);
    }

    /*--------.
    | Sending |
    `--------*/

    void
    Channel::_write(elle::Buffer const& packet)
    {
      this->_backend._write(packet, this->_id);
    }
  }
}
