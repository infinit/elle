#include <boost/foreach.hpp>

#include <elle/log.hh>

#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

#include <protocol/Channel.hh>
#include <protocol/ChanneledStream.hh>

ELLE_LOG_TRACE_COMPONENT("Infinit.Protocol");

namespace infinit
{
  namespace protocol
  {
    /*----------------.
    | Pretty printing |
    `----------------*/

    static std::ostream&
    operator << (std::ostream& s, ChanneledStream const& stream)
    {
      s << "infinit::protocol::ChanneledStream(" << &stream << ")";
      return s;
    }

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
      ELLE_LOG_TRACE_SCOPE("%s: read packet on channel %s", *this, channel->_id);
      int requested_channel = channel->_id;
      reactor::Thread* current = scheduler().current();
      while (true)
        {
          if (!channel->_packets.empty())
            {
              // FIXME: use helper to pop
              Packet packet(std::move(channel->_packets.front()));
              ELLE_LOG_TRACE("%s: %s available.", *this, packet);
              channel->_packets.pop_front();
              return packet;
            }
          ELLE_LOG_TRACE("%s: no packet available.", *this);
          if (!_reading)
            ELLE_LOG_TRACE("%s: reading packets.", *this)
              while (true)
                {
                  _reading = true;
                  Packet p(_backend.read());
                  int channel_id = _uint32_get(p);
                  Packet res;
                  {
                    // FIXME: size
                    // FIXME: copying like this SUCKS
                    char* buf = new char[p.size() - 4];
                    p.read(buf, p.size() - 4);
                    res.write(buf, p.size() - 4);
                    // FIXME: automatically flush when size or data is used
                    res.flush();
                    delete [] buf;
                  }
                  ELLE_LOG_TRACE("%s: received %s on channel %s.", *this, res, channel_id)
                  if (channel_id == requested_channel)
                    {
                      // Wake another thread so it can read future packets.
                      for (auto channel: _channels)
                        if (channel.second->_available.signal_one())
                          {
                            break;
                          }
                      _reading = false;
                      return res;
                    }
                  else
                    {
                      auto it = _channels.find(channel_id);
                      if (it != _channels.end())
                        {
                          it->second->_packets.push_back(std::move(res));
                          it->second->_available.signal_one();
                        }
                    else
                      {
                        _channels_new.push_back(Channel(*this, channel_id));
                        _channels_new.back()._packets.push_back(std::move(res));
                        _channel_available.signal_one();
                      }
                    }
                }
          else
            ELLE_LOG_TRACE("%s: reader already present, waiting.", *this)
              current->wait(channel->_available);
        }
    }

    Channel
    ChanneledStream::accept()
    {
      if (_channels_new.empty())
        {
          reactor::Thread* current = scheduler().current();
          current->wait(this->_channel_available);
        }
      assert(!_channels_new.empty());
      // FIXME: use helper to pop
      Channel res = std::move(_channels_new.front());
      _channels_new.pop_front();
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
      ELLE_LOG_TRACE_SCOPE("%s: send %s on channel %s", *this, packet, id);

      Packet backend_packet;
      _uint32_put(backend_packet, id);
      backend_packet.write(packet.data(), packet.size());
      backend_packet.flush();
      _backend.write(backend_packet);
    }
  }
}
