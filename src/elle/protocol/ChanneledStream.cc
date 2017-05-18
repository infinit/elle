#include <iostream>

#include <elle/find.hh>
#include <elle/log.hh>

#include <elle/reactor/scheduler.hh>
#include <elle/reactor/Thread.hh>

#include <elle/cryptography/random.hh>

#include <elle/protocol/Channel.hh>
#include <elle/protocol/ChanneledStream.hh>

ELLE_LOG_COMPONENT("elle.protocol.Channel");

namespace elle
{
  namespace protocol
  {
    /*-------------.
    | Construction |
    `-------------*/

    ChanneledStream::ChanneledStream(elle::reactor::Scheduler& scheduler,
                                     Stream& backend)
      : Super(scheduler)
      , _backend(backend)
      , _thread()
      , _exception()
      , _master(this->_handshake(backend))
      , _id_current(0)
      , _channels()
      , _channels_new()
      , _channel_available()
      , _default(*this)
    {
      this->_thread.reset(
        new reactor::Thread(
          elle::sprintf("%s", this), [this] { this->_read_thread(); }));
    }

    ChanneledStream::ChanneledStream(Stream& backend)
      : ChanneledStream(*elle::reactor::Scheduler::scheduler(), backend)
    {}

    ChanneledStream::~ChanneledStream()
    {
      try
      {
        this->_thread->terminate_now();
      }
      catch (...)
      {
        ELLE_ABORT("exception escaping %s destructor: %s",
                   this, elle::exception_string());
      }
    }

    void
    ChanneledStream::_read_thread()
    {
      ELLE_TRACE_SCOPE("%s: read packets", this);
      try
      {
        while (true)
        {
          auto p = this->_backend.read();
          int channel_id = this->uint32_get(p, this->version());
          // FIXME: The size of the packet isn't adjusted. This is cosmetic
          // though.
          if (auto it = elle::find(this->_channels, channel_id))
          {
            ELLE_DEBUG("received %f on channel %s", p, *it->second);
            it->second->_packets.put(std::move(p));
          }
          else
          {
            if (this->_master && channel_id > 0 ||
                !this->_master && channel_id < 0)
            {
              ELLE_TRACE("discard orphaned packet on channel %s", channel_id);
              continue;
            }
            Channel res(*this, channel_id);
            ELLE_DEBUG("received %f on new channel %s", p, channel_id);
            res._packets.put(std::move(p));
            this->_channels_new.put(std::move(res));
          }
        }
      }
      catch (elle::Error const&)
      {
        ELLE_TRACE("%s: read failed: %s", this, elle::exception_string());
        this->_channels_new.raise(std::current_exception());
        this->_default._packets.raise(std::current_exception());
        for (auto& c: this->_channels)
          c.second->_packets.raise(std::current_exception());
        this->_exception = std::current_exception();
      }
    }

    bool
    ChanneledStream::_handshake(Stream& backend)
    {
      while (true)
      {
        ELLE_TRACE_SCOPE("%s: handshake to determine master", *this);
        char mine = elle::cryptography::random::generate<char>();
        char his;
        {
          elle::Buffer p;
          p.append(&mine, 1);
          backend.write(p);
          ELLE_DEBUG("%s: my roll: %d", *this, (int)mine);
        }
        {
          elle::Buffer p(backend.read());
          if (signed(p.size()) != 1)
            elle::err("invalid hanshake packet size: %s", p.size());
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
    ChanneledStream::_read()
    {
      return this->_default.read();
    }

    Channel
    ChanneledStream::accept()
    {
      ELLE_TRACE_SCOPE("%s: accept channel", this);
      return this->_channels_new.get();
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

      auto backend_packet = elle::Buffer{};
      this->uint32_put(backend_packet, id, this->version());
      backend_packet.append(packet.contents(), packet.size());
      this->_backend.write(backend_packet);
    }

    /*--------.
    | Version |
    `--------*/

    const elle::Version&
    ChanneledStream::version() const
    {
      return this->_backend.version();
    }

    /*----------.
    | Printable |
    `----------*/

    void
    ChanneledStream::print(std::ostream& stream) const
    {
      elle::fprintf(stream, "%s(%s)", elle::type_info(*this), this->_backend);
    }
  }
}
