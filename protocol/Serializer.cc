#include <arpa/inet.h>

#include <elle/log.hh>

#include <protocol/Serializer.hh>

ELLE_LOG_TRACE_COMPONENT("Infinit.Protocol");

namespace infinit
{
  namespace protocol
  {
    /*----------------.
    | Pretty printing |
    `----------------*/

    static std::ostream&
    operator << (std::ostream& s, Serializer const& ps)
    {
      s << "infinit::protocol::Serializer(" << &ps << ")";
      return s;
    }

    /*-------------.
    | Construction |
    `-------------*/

    Serializer::Serializer(reactor::Scheduler& scheduler, std::iostream& stream)
      : Super(scheduler)
      , _stream(stream)
    {}

    /*----------.
    | Receiving |
    `----------*/

    Packet
    Serializer::read()
    {
      ELLE_LOG_TRACE("%s: read packet", *this)
        {
          uint32_t size(_uint32_get(_stream));
          ELLE_LOG_TRACE("%s: packet size: %s, reading body", *this, size);
          Packet res(size);
          _stream.read(reinterpret_cast<char*>(res._data), size);
          ELLE_LOG_TRACE("%s: got full packet", *this);
          return res;
        }
    }

    /*--------.
    | Sending |
    `--------*/
    void
    Serializer::write(Packet& packet)
    {
      ELLE_LOG_TRACE("%s: send packet of size %s",
                     *this, packet._data_size)
        {
          _uint32_put(_stream, packet._data_size);
          _stream.write(reinterpret_cast<char*>(packet._data),
                        packet._data_size);
        }
      _stream.flush();
    }
  }
}
