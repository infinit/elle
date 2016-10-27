#include <boost/foreach.hpp>

#include <elle/log.hh>

#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

#include <cryptography/random.hh>

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

    ChanneledStream::ChanneledStream(reactor::Scheduler& scheduler,
                                     Stream& backend)
      : Super(scheduler)
      , _master(this->_handshake(backend))
      , _id_current(0)
      , _reading(false)
      , _backend(backend)
      , _channels()
      , _channels_new()
      , _channel_available()
      , _default(*this)
    {}

    ChanneledStream::ChanneledStream(Stream& backend)
      : ChanneledStream(*reactor::Scheduler::scheduler(), backend)
    {}

    bool
    ChanneledStream::_handshake(Stream& backend)
    {
      while (true)
      {
        ELLE_TRACE_SCOPE("%s: handshake to determine master", *this);
        char mine = infinit::cryptography::random::generate<char>();
        char his;
        {
          elle::Buffer p;
          p.append(&mine, 1);
          backend.write(p);
          ELLE_DEBUG("%s: my roll: %d", *this, (int)mine);
        }
        {
          elle::Buffer p(backend.read());
          ELLE_ASSERT_EQ(1, (signed)p.size());
          his = p.contents()[0];
          ELLE_DEBUG("%s: his roll: %d", *this, (int)his);
        }
        if (mine != his)
        {
          bool master = mine > his;
          ELLE_TRACE("%s: %s", *this, master ? "master" : "slave");
          return master;
        }
        else
          ELLE_DEBUG("rolls are equal, restart handshake");
      }
    }

    /*----.
    | IDs |
    `----*/

    int
    ChanneledStream::_id_generate()
    {
      int res = this->_id_current;
      if (this->_master)
      {
        ++this->_id_current;
        if (this->_id_current < 0)
          this->_id_current = 1;
      }
      else
      {
        --this->_id_current;
        if (this->_id_current > 0)
          this->_id_current = -1;
      }
      return res;
    }

    /*----------.
    | Receiving |
    `----------*/

    elle::Buffer
    ChanneledStream::read()
    {
      return this->_default.read();
    }

    elle::Buffer
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
              elle::Buffer packet(std::move(channel->_packets.front()));
              channel->_packets.pop_front();
              ELLE_TRACE("%s: %f available.", *this, packet);
              return packet;
            }
          ELLE_DEBUG("%s: no packet available.", *this);
          if (!_reading)
            this->_read(false, requested_channel);
          else
            ELLE_DEBUG("%s: reader already present, waiting.", *this)
              current->wait(channel->_available);
        }
    }

    void
    ChanneledStream::_read(bool new_channel, int requested_channel)
    {
      ELLE_TRACE_SCOPE("%s: reading packets.", *this);
      ELLE_ASSERT(!_reading);
      try
      {
        bool goon = true;
        while (goon)
        {
          elle::With<reactor::Thread::NonInterruptible>() << [&]
          {
            this->_reading = true;
            elle::Buffer p(this->_backend.read());
            if (p.size() < 4)
              throw elle::Exception("packet is too small for channel id");
            int channel_id = uint32_get(p);
            // FIXME: The size of the packet isn't
            // adjusted. This is cosmetic though.
            auto it = this->_channels.find(channel_id);
            if (it != this->_channels.end())
            {
              ELLE_DEBUG("%s: received %f on existing %s (requested %s).",
                         *this, p, *it->second, requested_channel);
              it->second->_packets.push_back(std::move(p));
              if (channel_id == requested_channel)
                {goon = false; return;}
              else
                it->second->_available.signal_one();
            }
            else
            {
              ELLE_ASSERT(channel_id != requested_channel);
              Channel res(*this, channel_id);
              ELLE_DEBUG("%s: received %f on brand new %s (requested %s).",
                         *this, p, res, requested_channel);
              res._packets.push_back(std::move(p));
              this->_channels_new.push_back(std::move(res));
              if (new_channel)
                {goon = false; return;}
              else
                this->_channel_available.signal_one();
            }
          };
        }
        // Exited loop, Wake another thread so it can read future packets.
        this->_reading = false;
        for (auto channel: this->_channels)
          if (channel.second->_available.signal_one())
            return;
        this->_channel_available.signal_one();
      }
      catch (std::exception const& ex)
      {
        auto e = std::current_exception();
        // Wake another thread so it fails too.
        ELLE_DEBUG_SCOPE("%s: read failed, wake next thread: %s.", *this,
                         ex.what());
        this->_reading = false;
        bool woken = false;
        for (auto channel: this->_channels)
          if (channel.second->_available.signal_one())
          {
            woken = true;
            break;
          }
        if (!woken)
          this->_channel_available.signal_one();
        std::rethrow_exception(e);
      }
    }

    Channel
    ChanneledStream::accept()
    {
      ELLE_TRACE_SCOPE("%s: wait for incoming channel", *this);
      while (true)
      {
        ELLE_DEBUG("%s: no channel available, waiting", *this);
        if (!this->_reading)
          this->_read(true, 0);
        else
        {
          ELLE_DEBUG("%s: reader already present, waiting.", *this);
          reactor::Thread* current = scheduler().current();
          current->wait(this->_channel_available);
        }
        if (this->_channels_new.empty())
          continue;
        // FIXME: use helper to pop
        Channel res = std::move(this->_channels_new.front());
        this->_channels_new.pop_front();
        if (this->_master && res.id() > 0 || !this->_master && res.id() < 0)
        {
          ELLE_TRACE("%s: discard orphaned packet on channel %s",
                     this, res.id());
          continue;
        }
        ELLE_TRACE("%s: got %s", this, res);
        return res;
      }
    }

    /*--------.
    | Sending |
    `--------*/

    void
    ChanneledStream::_write(elle::Buffer const& packet)
    {
      this->_default.write(packet);
    }

    void
    ChanneledStream::_write(elle::Buffer const& packet, int id)
    {
      ELLE_TRACE_SCOPE("%s: send %f on channel %s", *this, packet, id);

      elle::Buffer backend_packet;
      uint32_put(backend_packet, id);
      backend_packet.append(packet.contents(), packet.size());
      this->_backend.write(backend_packet);
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
