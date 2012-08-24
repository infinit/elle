#include <arpa/inet.h>

#include <elle/log.hh>

#include <protocol/Serializer.hh>

ELLE_LOG_COMPONENT("infinit.protocol.Serializer");

namespace infinit
{
  namespace protocol
  {
    /*-------------.
    | Construction |
    `-------------*/

    Serializer::Serializer(reactor::Scheduler& scheduler, std::iostream& stream)
      : Super(scheduler)
      , _stream(stream)
      , _lock_write()
      , _lock_read()
    {}

    /*----------.
    | Receiving |
    `----------*/

    Packet
    Serializer::read()
    {
      reactor::Lock lock(scheduler(), _lock_read);
      ELLE_TRACE("%s: read packet", *this)
        {
          uint32_t size(_uint32_get(_stream));
          ELLE_DEBUG("%s: packet size: %s, reading body", *this, size);
          Packet res(size);
          _stream.read(reinterpret_cast<char*>(res._data), size);
          ELLE_TRACE("%s: got %s", *this, res);
          return res;
        }
    }

    /*--------.
    | Sending |
    `--------*/
    void
    Serializer::_write(Packet& packet)
    {
      reactor::Lock lock(scheduler(), _lock_write);
      ELLE_TRACE("%s: send %s", *this, packet)
        {
          _uint32_put(_stream, packet._data_size);
          _stream.write(reinterpret_cast<char*>(packet._data),
                        packet._data_size);
        }
      _stream.flush();
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Serializer::print(std::ostream& stream) const
    {
      stream << "Serializer " << this;
    }
  }
}
