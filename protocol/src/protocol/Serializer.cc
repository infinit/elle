#ifdef INFINIT_WINDOWS
# include <winsock2.h>
#else
# include <arpa/inet.h>
#endif

#include <elle/log.hh>

#include <cryptography/hash.hh>

#include <reactor/scheduler.hh>
#include <reactor/exception.hh>
#include <reactor/Barrier.hh>

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

    Serializer::Serializer(std::iostream& stream, bool checksum):
      Serializer(*reactor::Scheduler::scheduler(), stream, checksum)
    {}

    Serializer::Serializer(reactor::Scheduler& scheduler,
                           std::iostream& stream,
                           bool checksum):
      Super(scheduler),
      _stream(stream),
      _lock_write(),
      _lock_read(),
      _checksum(checksum)
    {}

    /*----------.
    | Receiving |
    `----------*/

    elle::Buffer
    Serializer::read()
    {
      reactor::Lock lock(_lock_read);
      elle::IOStreamClear clearer(_stream);
      int state = 0;
      elle::SafeFinally state_checker([&] {
        if (state == 1)
          ELLE_ERR("serializer::read interrupted in unsafe state");
      });
      ELLE_TRACE("%s: read packet", *this)
      {
        elle::Buffer hash;
        if (_checksum)
        {
          uint32_t hash_size(_uint32_get(_stream));
          ELLE_DUMP("%s: checksum size: %s", *this, hash_size);

          hash.size(hash_size);
          _stream.read(reinterpret_cast<char*>(hash.mutable_contents()),
                       hash_size);
          ELLE_DUMP("%s: checksum: %s", *this, hash);
        }
        uint32_t size(_uint32_get(_stream));
        ELLE_DEBUG("%s: packet size: %s", *this, size);
        state = 1;
        elle::Buffer packet(static_cast<std::size_t>(size));
        // read the full packet even if terminated to keep the stream
        // in a consistent state
        int nread = 0;
        while (nread < signed(size))
        {
          try
          {
            elle::IOStreamClear clearer(_stream);
            std::streamsize r = std::readsome(_stream,
              (char*)(packet.mutable_contents()) +  nread, size - nread);
            nread += r;
          }
          catch (reactor::Terminate const&)
          {
            while (nread < signed(size))
            {
              elle::IOStreamClear clearer(_stream);
              std::streamsize r = std::readsome(_stream,
                (char*)(packet.mutable_contents()) +  nread, size - nread);
              nread += r;
            }
            state = 2;
            throw;
          }
        }
        state = 2;
        ELLE_DUMP("%s: packet data %s", *this, packet);
        // Check hash.
        if (_checksum)
        {
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
          auto _hash_local =
            infinit::cryptography::hash(
              infinit::cryptography::Plain(
                elle::WeakBuffer(packet.mutable_contents(),
                                 packet.size())),
              infinit::cryptography::Oneway::sha1);
          auto hash_local(_hash_local.buffer());
#else
          auto hash_local =
            infinit::cryptography::hash(
              elle::ConstWeakBuffer(packet.contents(),
                                    packet.size()),
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
        }
        return packet;
      }
    }

    /*--------.
    | Sending |
    `--------*/
    void
    Serializer::_write(elle::Buffer& packet)
    {
      // The write must not be interrupted, otherwise it will break
      // the serialization protocol.
      reactor::Thread::NonInterruptible ni;
      reactor::Lock lock(_lock_write);
      elle::IOStreamClear clearer(_stream);
      ELLE_TRACE("%s: send %s", *this, packet)
      if (_checksum)
      {
#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
        auto _hash =
        infinit::cryptography::hash(
          infinit::cryptography::Plain(
            elle::WeakBuffer(packet.mutable_contents(),
              packet.size())),
          infinit::cryptography::Oneway::sha1);
        auto hash(_hash.buffer());
#else
        auto hash =
        infinit::cryptography::hash(
          elle::ConstWeakBuffer(packet.contents(),
                                packet.size()),
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
      }
      auto size = packet.size();
      ELLE_DUMP("%s: send packet size %s", *this, size)
      _uint32_put(_stream, size);
      ELLE_DUMP("%s: send packet data", *this)
      _stream.write(reinterpret_cast<char*>(packet.mutable_contents()),
        packet.size());
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
