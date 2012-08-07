#include <arpa/inet.h>

#include <elle/log.hh>

#include <protocol/Serializer.hh>

ELLE_LOG_COMPONENT("Infinit.Protocol");

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
          ELLE_TRACE("%s: packet size: %s, reading body", *this, size);
          Packet res(size);
          _stream.read(reinterpret_cast<char*>(res._data), size);
          ELLE_TRACE("%s: got full packet", *this);
          return res;
        }
    }

    /*--------.
    | Sending |
    `--------*/
    void
    Serializer::write(Packet& packet)
    {
      reactor::Lock lock(scheduler(), _lock_write);
      ELLE_TRACE("%s: send packet of size %s",
                     *this, packet._data_size)
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
