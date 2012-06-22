#include <arpa/inet.h>

#include <elle/log.hh>

#include <protocol/PacketStream.hh>

ELLE_LOG_TRACE_COMPONENT("Infinit.Protocol");

namespace infinit
{
  namespace protocol
  {
    /*----------------.
    | Pretty printing |
    `----------------*/

    static std::ostream&
    operator << (std::ostream& s, PacketStream const& ps)
    {
      s << "infinit::protocol::StreamBuffer(" << &ps << ")";
      return s;
    }

    /*-------------.
    | Construction |
    `-------------*/

    PacketStream::PacketStream(std::iostream& stream)
      : _stream(stream)
    {}

    /*----------.
    | Receiving |
    `----------*/

    Packet
    PacketStream::read()
    {
      ELLE_LOG_TRACE("%s: read packet", *this)
        {
          uint32_t size;
          _stream.read(reinterpret_cast<char*>(&size), sizeof(size));
          size = ntohl(size);
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
    PacketStream::write(Packet& packet)
    {
      ELLE_LOG_TRACE("%s: send packet of size %s",
                     *this, packet._data_size)
        {
          uint32_t size = htonl(packet._data_size);
          _stream.write(reinterpret_cast<char*>(&size),
                        sizeof(size));
          _stream.write(reinterpret_cast<char*>(packet._data),
                        packet._data_size);
        }
      _stream.flush();
    }
  }
}
