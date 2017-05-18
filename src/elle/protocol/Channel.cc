#include <elle/protocol/Channel.hh>

#include <elle/algorithm.hh>
#include <elle/log.hh>
#include <elle/protocol/ChanneledStream.hh>

ELLE_LOG_COMPONENT("elle.protocol.Channel");

namespace elle
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
      ELLE_ASSERT(!elle::contains(this->_backend._channels, this->_id));
      this->_backend._channels[this->_id] = this;
    }

    Channel::Channel(ChanneledStream& backend)
      : Channel(backend, backend._id_generate())
    {}

    Channel::Channel(Channel&& source)
      : Super(source.scheduler())
      , _backend(source._backend)
      , _id(source._id)
      , _packets(std::move(source._packets))
      , _available(std::move(source._available))
    {
      source._id = 0;
      ELLE_ASSERT(elle::contains(this->_backend._channels, this->_id));
      this->_backend._channels[this->_id] = this;
    }

    Channel::~Channel()
    {
      if (this->_id)
      {
        ELLE_TRACE_SCOPE("%s: close", this);
        if (!this->_packets.empty())
          ELLE_TRACE("closing with %s packets in queue",
                     this->_packets.size());
        if (!this->_available.waiters().empty())
          ELLE_TRACE("closing with %s waiters in line",
                     this->_available.waiters().size());
        ELLE_ASSERT(elle::contains(this->_backend._channels, this->_id));
        this->_backend._channels.erase(this->_id);
      }
    }

    /*--------.
    | Version |
    `--------*/
    elle::Version const&
    Channel::version() const
    {
      return this->_backend.version();
    }


    /*---------.
    | Printing |
    `---------*/

    void
    Channel::print(std::ostream& s) const
    {
      elle::fprintf(s, "Channel(%s, %f)",
                    this->_id, this->_backend);
    }

    /*----------.
    | Receiving |
    `----------*/

    elle::Buffer
    Channel::_read()
    {
      return this->_packets.get();
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
