#include <arpa/inet.h>

#include <elle/log.hh>

#include <cryptography/oneway.hh>

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
      reactor::Lock lock(_lock_read);
      ELLE_TRACE("%s: read packet", *this)
      {
        uint32_t hash_size(_uint32_get(_stream));
        ELLE_DUMP("%s: checksum size: %s", *this, hash_size);

        elle::Buffer hash(hash_size);
        _stream.read(reinterpret_cast<char*>(hash.mutable_contents()),
                     hash_size);
        ELLE_DUMP("%s: checksum: %s", *this, hash);

        uint32_t size(_uint32_get(_stream));
        ELLE_DEBUG("%s: packet size: %s", *this, size);

        Packet packet(size);
        _stream.read(reinterpret_cast<char*>(packet._data), size);
        ELLE_DUMP("%s: packet data %s", *this, packet);

        // Check hash.
        auto hash_local = cryptography::oneway::hash(
          cryptography::Plain(elle::WeakBuffer(packet._data,
                                               packet._data_size)),
          cryptography::oneway::Algorithm::sha1);
        ELLE_DUMP("%s: local checksum: %s", *this, hash_local);
        if (hash_local.buffer() != hash)
        {
          ELLE_ERR("%s: wrong packet checksum", *this);
          throw elle::Exception("wrong checksum");
        }
        else
          ELLE_DUMP("%s: checksum match", *this);

        return packet;
      }
    }

    /*--------.
    | Sending |
    `--------*/
    void
    Serializer::_write(Packet& packet)
    {
      reactor::Lock lock(_lock_write);
      ELLE_TRACE("%s: send %s", *this, packet)
      {
        auto hash = cryptography::oneway::hash(
          cryptography::Plain(elle::WeakBuffer(packet._data,
                                               packet._data_size)),
          cryptography::oneway::Algorithm::sha1);
          auto hash_size = hash.buffer().size();
        ELLE_DUMP("%s: send checksum size: %s", *this, hash_size)
          _uint32_put(_stream, hash_size);
        ELLE_DUMP("%s: send checksum: %s", *this, hash.buffer())
        {
          auto data = reinterpret_cast<char*>(hash.buffer().mutable_contents());
          _stream.write(data, hash_size);
        }
        auto size = packet._data_size;
        ELLE_DUMP("%s: send packet size %s", *this, size)
          _uint32_put(_stream, size);
        ELLE_DUMP("%s: send packet data", *this)
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
