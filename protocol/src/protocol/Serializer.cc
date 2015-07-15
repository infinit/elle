#ifdef INFINIT_WINDOWS
# include <winsock2.h>
#else
# include <arpa/inet.h>
#endif

#include <elle/log.hh>

#include <cryptography/hash.hh>

#include <reactor/scheduler.hh>

#include <protocol/Serializer.hh>
#include <protocol/exceptions.hh>

ELLE_LOG_COMPONENT("infinit.protocol.Serializer");

namespace infinit
{
  namespace protocol
  {
    /*-------------.
    | Construction |
    `-------------*/

    Serializer::Serializer(std::iostream& stream):
      Serializer(*reactor::Scheduler::scheduler(), stream)
    {}

    Serializer::Serializer(reactor::Scheduler& scheduler,
                           std::iostream& stream):
      Super(scheduler),
      _stream(stream),
      _lock_write(),
      _lock_read()
    {}

    /*----------.
    | Receiving |
    `----------*/

    Packet
    Serializer::read()
    {
      reactor::Lock lock(_lock_read);
      elle::IOStreamClear clearer(_stream);
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
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        auto _hash_local =
          infinit::cryptography::hash(
            infinit::cryptography::Plain(
              elle::WeakBuffer(packet._data,
                               packet._data_size)),
            infinit::cryptography::Oneway::sha1);
        auto hash_local(_hash_local.buffer());
#else
        auto hash_local =
          infinit::cryptography::hash(
            elle::ConstWeakBuffer(packet._data,
                                  packet._data_size),
            infinit::cryptography::Oneway::sha1);
#endif
        ELLE_DUMP("%s: local checksum: %s", *this, hash_local);
        if (hash_local != hash)
        {
          ELLE_ERR("%s: wrong packet checksum", *this);
          throw ChecksumError();
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
      elle::IOStreamClear clearer(_stream);
      ELLE_TRACE("%s: send %s", *this, packet)
      {
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        auto _hash =
          infinit::cryptography::hash(
            infinit::cryptography::Plain(
              elle::WeakBuffer(packet._data,
                               packet._data_size)),
            infinit::cryptography::Oneway::sha1);
        auto hash(_hash.buffer());
#else
        auto hash =
          infinit::cryptography::hash(
            elle::ConstWeakBuffer(packet._data,
                                  packet._data_size),
            infinit::cryptography::Oneway::sha1);
#endif
          auto hash_size = hash.size();
        ELLE_DUMP("%s: send checksum size: %s", *this, hash_size)
          _uint32_put(_stream, hash_size);
        ELLE_DUMP("%s: send checksum: %s", *this, hash)
        {
          auto data = reinterpret_cast<char*>(hash.mutable_contents());
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
