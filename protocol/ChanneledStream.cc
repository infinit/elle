#include <boost/foreach.hpp>

#include <elle/log.hh>

#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

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

    ChanneledStream::ChanneledStream(reactor::Scheduler& scheduler, Stream& backend, bool master)
      : Super(scheduler)
      , _master(master)
      , _id_current(0)
      , _reading(false)
      , _backend(backend)
      , _channels()
      , _channels_new()
      , _channel_available()
      , _default(*this)
    {}

    /*----.
    | IDs |
    `----*/

    int
    ChanneledStream::_id_generate()
    {
      int res = _id_current;
      if (_master)
        {
          ++_id_current;
          if (_id_current < 0)
            _id_current = 1;
        }
      else
        {
          --_id_current;
          if (_id_current > 0)
            _id_current = -1;
        }
      return res;
    }

    /*----------.
    | Receiving |
    `----------*/

    Packet
    ChanneledStream::read()
    {
      return _default.read();
    }

    Packet
    ChanneledStream::_read(Channel* channel)
    {
      ELLE_TRACE_SCOPE("%s: read packet on channel %s", *this, channel->_id);
      int requested_channel = channel->_id;
      reactor::Thread* current = scheduler().current();
      while (true)
        {
          if (!channel->_packets.empty())
            {
              // FIXME: use helper to pop
              Packet packet(std::move(channel->_packets.front()));
              ELLE_TRACE("%s: %s available.", *this, packet);
              channel->_packets.pop_front();
              return packet;
            }
          ELLE_TRACE("%s: no packet available.", *this);
          if (!_reading)
            _read(false, requested_channel);
          else
            ELLE_TRACE("%s: reader already present, waiting.", *this)
              current->wait(channel->_available);
        }
    }

    void
    ChanneledStream::_read(bool channel, int requested_channel)
    {
      ELLE_TRACE_SCOPE("%s: reading packets.", *this);
      while (true)
        {
          _reading = true;
          Packet p(_backend.read());
          int channel_id = _uint32_get(p);
          // FIXME: The size of the packet isn't
          // adjusted. This is cosmetic though.
          ELLE_TRACE("%s: received %s on channel %s.", *this, p, channel_id);
          auto it = _channels.find(channel_id);
          if (it != _channels.end())
            {
              it->second->_packets.push_back(std::move(p));
              if (channel_id == requested_channel)
                break;
              else
                  it->second->_available.signal_one();
            }
          else
            {
              Channel res(*this, channel_id);
              res._packets.push_back(std::move(p));
              _channels_new.push_back(std::move(res));
              if (channel)
                break;
              else
                _channel_available.signal_one();
            }
        }
      // Wake another thread so it can read future packets.
      _reading = false;
      for (auto channel: _channels)
        if (channel.second->_available.signal_one())
          return;
      _channel_available.signal_one();
    }

    Channel
    ChanneledStream::accept()
    {
      ELLE_TRACE_SCOPE("%s: wait for incoming channel", *this);
      while (_channels_new.empty())
        {
          ELLE_TRACE_SCOPE("%s: no channel available, waiting", *this);
          if (!_reading)
            _read(true, 0);
          else
            {
              ELLE_TRACE_SCOPE("%s: reader already present, waiting.", *this);
              reactor::Thread* current = scheduler().current();
              current->wait(this->_channel_available);
            }
        }
      assert(!_channels_new.empty());
      // FIXME: use helper to pop
      Channel res = std::move(_channels_new.front());
      _channels_new.pop_front();
      ELLE_TRACE_SCOPE("%s: got channel %s", *this, res);
      return res;
    }

    /*--------.
    | Sending |
    `--------*/

    void
    ChanneledStream::write(Packet& packet)
    {
      _default.write(packet);
    }

    void
    ChanneledStream::_write(Packet& packet, int id)
    {
      ELLE_TRACE_SCOPE("%s: send %s on channel %s", *this, packet, id);

      Packet backend_packet;
      _uint32_put(backend_packet, id);
      backend_packet.write(packet._data, packet.size());
      backend_packet.flush();
      _backend.write(backend_packet);
    }

    /*----------.
    | Printable |
    `----------*/

    void
    ChanneledStream::print(std::ostream& stream) const
    {
      stream << "ChanneledStream " << this;
    }
  }
}
