#ifdef INFINIT_WINDOWS
# include <winsock2.h>
#else
# include <arpa/inet.h>
#endif

#include <elle/Buffer.hh>
#include <elle/log.hh>

#include <elle/cryptography/hash.hh>

#include <elle/serialization/binary.hh>
#include <elle/serialization/json.hh>

#include <elle/reactor/scheduler.hh>
#include <elle/reactor/exception.hh>
#include <elle/reactor/Barrier.hh>
#include <elle/reactor/network/socket.hh>
#include <elle/reactor/network/utp-socket.hh>

#include <elle/protocol/Serializer.hh>
#include <elle/protocol/exceptions.hh>

ELLE_LOG_COMPONENT("elle.protocol.Serializer");

namespace elle
{
  namespace protocol
  {
    /*---------------.
    | Implementation |
    `---------------*/

    // Read a chunk from the given stream.
    // The data struct is:
    // - the size: n bytes
    // - the content: $size bytes.
    static
    void
    read(Serializer::Inner& stream,
         elle::Buffer& content,
         uint32_t size,
         uint32_t offset = 0)
    {
      ELLE_DEBUG_SCOPE("read %s bytes from %s at offset %s",
                       size, stream, offset);
      // read the full packet even if terminated to keep the stream
      // in a consistent state
      int nread = 0;
      auto* beginning = (char*) content.mutable_contents() + offset;
      while (nread < signed(size))
      {
        char* where = beginning + nread;
        try
        {
          ELLE_DUMP_SCOPE("read from %s", nread);
          elle::IOStreamClear clearer(stream);
          nread += std::readsome(stream, where, size - nread);
          if (stream.eof())
            throw Serializer::EOF();
        }
        catch (Serializer::EOF const&)
        {
          throw;
        }
        catch (...)
        {
          ELLE_TRACE("reading %s interrupted", stream);
          while (nread < signed(size))
          {
            ELLE_DUMP_SCOPE("read from %s", nread);
            char* where = beginning + nread;
            if (stream.eof())
              throw;
            stream.clear();
            std::streamsize r = std::readsome(stream, where, size - nread);
            ELLE_DEBUG("read: %sB", r);
            nread += r;
          }
          throw;
        }
      }
    }

    static
    elle::Buffer
    read(Serializer::Inner& stream,
         elle::Version const& version,
         boost::optional<uint32_t> size = boost::none)
    {
      ELLE_DEBUG_SCOPE("read %s (size: %s)", stream, size);
      if (!size)
        size = Serializer::Super::uint32_get(stream, version);
      ELLE_DUMP("expected size: %s", *size);
      elle::Buffer content(*size);
      read(stream, content, *size);
      return content;
    }
    // Return the sha1 of a given buffer.
    static
    elle::Buffer
    compute_checksum(elle::Buffer const& content)
    {
      ELLE_DUMP("compute checksum of '%x'", content);
      auto hash = elle::cryptography::hash(
        elle::ConstWeakBuffer(content.contents(),
                              content.size()),
        elle::cryptography::Oneway::sha1);
      ELLE_DUMP("checksum: '%x'", hash);
      return hash;
    }

    // Make sure the given buffer checksum match the given checksum.
    static
    void
    enforce_checksums_equal(elle::Buffer const& content,
                            elle::Buffer const& expected_checksum)
    {
      ELLE_DUMP_SCOPE("compare '%x' checksum with expected '%x'",
                       content, expected_checksum);
      auto checksum = compute_checksum(content);
      ELLE_DUMP("checksum: '%x'", checksum);
      if (checksum != expected_checksum)
      {
        ELLE_ERR("wrong packet checksum")
          throw ChecksumError();
      }
    }

    static
    void
    write(Serializer::Inner& stream,
          elle::Version const& version,
          elle::Buffer const& content,
          bool write_size = true,
          elle::Buffer::Size offset = 0,
          boost::optional<elle::Buffer::Size> size = boost::none)
    {
      elle::Buffer::Size to_send = size ? size.get() : content.size();
      ELLE_DEBUG_SCOPE("write %s '%f' at offset %s (write size: %s)",
                       to_send == content.size()
                       ? std::string{"whole"}
                       : elle::sprintf("%s bytes from", to_send),
                       content,
                       offset,
                       write_size);
      if (write_size)
        Serializer::Super::uint32_put(stream, to_send, version);
      stream.write(
        reinterpret_cast<char*>(content.mutable_contents()) + offset,
        to_send);
    }

    enum Control: unsigned char
    {
      keep_going = 0,
      interrupt = 1,
      message = 2,
      max = message,
    };

    // Check control byte.
    static
    Control
    read_control(Serializer::Inner& stream)
    {
      ELLE_DUMP_SCOPE("read control");
      char control = static_cast<char>(Control::max + 1);
      stream.read(&control, 1);
      if (control > Control::max)
      {
        ELLE_ERR("%s: invalid control byte: 0x%x",
                 stream, static_cast<int>(control));
        elle::err<protocol::Error>(
          "invalid control byte: 0x%x", static_cast<int>(control));
      }
      else
        ELLE_DUMP("control: '%x'", (int) control);
      return (Control) control;
    }

    static
    void
    write_control(Serializer::Inner& stream,
                  Control control)
    {
      ELLE_DUMP_SCOPE("send control %s", (int) control);
      char c = static_cast<char>(control);
      stream.write(&c, 1);
    }

    static
    void
    ignore_message(Serializer::Inner& stream, elle::Version const& version)
    {
      // Version 0.2.0 handle but ignores messages.
      auto res = elle::protocol::read(stream, version);
      ELLE_WARN("%f was ignored", res);
    }

    class Serializer::Impl
    {
    public:
      Impl(std::iostream& stream,
           elle::Buffer::Size chunk_size,
           bool checksum,
           elle::Version const& version)
        : _stream(stream)
        , _chunk_size(chunk_size)
        , _checksum(checksum)
        , _version(version)
        , _lock_write()
        , _lock_read()
      {}

      virtual
      ~Impl() = default;

    public:
      elle::Buffer
      read()
      {
        while (true)
        {
          try
          {
            elle::reactor::Lock lock(this->_lock_read);
            elle::IOStreamClear clearer(this->_stream);
            return this->_read();
          }
          catch (InterruptionError const&)
          {
          }
        }
        ELLE_ERR("exit reading")
          elle::unreachable();
      }

      elle::Buffer
      _read()
      {
        // Make sure the stream isn't terminated.
        if (elle::With<elle::reactor::Thread::Interruptible>() << [&]
           {
             return this->_stream.peek();
           } == std::iostream::traits_type::eof())
          throw Serializer::EOF();
        elle::Buffer hash;
        if (this->_checksum)
        {
          ELLE_DEBUG("read checksum")
            if (this->version() >= elle::Version(0, 2, 0))
              hash = elle::protocol::read(this->_stream, this->version(), {});
            else
              hash = elle::protocol::read(this->_stream, {});
        }
        auto packet = [&]
        {
          if (this->version() >= elle::Version(0, 2, 0))
          {
            // Get the total size.
            uint32_t total_size(Serializer::Super::uint32_get(this->_stream,
                                                              this->version()));
            ELLE_DEBUG("packet size: %s", total_size);
            elle::Buffer packet(static_cast<std::size_t>(total_size));
            elle::Buffer::Size offset = 0;
            while (true)
            {
              uint32_t size = std::min(total_size - offset, this->_chunk_size);
              ELLE_DEBUG("read chunk of size %s", size);
              elle::protocol::read(this->_stream, packet, size, offset);
              offset += size;
              ELLE_ASSERT_LTE(offset, total_size);
              if (offset >= total_size)
                break;
              while (true)
                switch (read_control(this->_stream))
                {
                  case Control::keep_going:
                    goto keep_going;
                  case Control::interrupt:
                    throw InterruptionError();
                  case Control::message:
                    ignore_message(this->_stream, this->version());
                    break;
                }
              keep_going:;
            }
            return packet;
          }
          else
            return elle::protocol::read(this->_stream, {});
        }();
        ELLE_DUMP("packet content: %s", packet);
        // Check checksums match.
        if (this->_checksum)
          enforce_checksums_equal(packet, hash);
        return packet;
      }

      void
      write(elle::Buffer const& packet)
      {
        elle::reactor::Lock lock(this->_lock_write);
        elle::IOStreamClear clearer(this->_stream);
        this->_write(packet);
      }

      void
      _write(elle::Buffer const& packet)
      {
        if (this->_checksum)
          elle::With<elle::reactor::Thread::NonInterruptible>() << [&]
          {
            // Compute and send checksum.
            {
              auto hash = compute_checksum(packet);
              ELLE_DEBUG("send checksum: 0x%x", hash)
                elle::protocol::write(this->_stream, this->version(), hash);
            }
          };
        if (this->version() >= elle::Version(0, 2, 0))
        {
          elle::Buffer::Size offset = 0;
          try
          {
            auto send = [&]
              {
                auto to_send = std::min(this->_chunk_size, packet.size() - offset);
                ELLE_DEBUG("send actual data: %s", to_send)
                elle::protocol::write(
                  this->_stream,
                  this->version(), packet, false, offset, to_send);
                offset += to_send;
                this->_stream.flush();
              };
            {
              elle::With<elle::reactor::Thread::NonInterruptible>() << [&]
              {
                // Send the size.
                {
                  auto size = packet.size();
                  ELLE_DEBUG("send packet size %s", size)
                    Serializer::Super::uint32_put(this->_stream, size, this->version());
                }
                // Send first chunk
                send();
              };
            }
            while (offset < packet.size())
            {
              ELLE_DEBUG("writing control: o=%s, size=%s", offset, packet.size());
              elle::With<elle::reactor::Thread::NonInterruptible>() << [&]
              {
                write_control(this->_stream, Control::keep_going);
                send();
              };
            }
          }
          catch (elle::reactor::Terminate const&)
          {
            if (offset < packet.size())
            {
              ELLE_DEBUG("interrupted after sending %s bytes over %s",
                         offset, packet.size());
              write_control(this->_stream, Control::interrupt);
              this->_stream.flush();
            }
            throw;
          }
        }
        else
          // The write must not be interrupted, otherwise it will break
          // the serialization protocol.
          elle::With<elle::reactor::Thread::NonInterruptible>() << [&]
          {
            ELLE_DEBUG("send actual data")
              elle::protocol::write(this->_stream, this->version(), packet);
            this->_stream.flush();
          };
      }
    private:
      ELLE_ATTRIBUTE_RX(std::iostream&, stream, protected);
      ELLE_ATTRIBUTE(elle::Buffer::Size, chunk_size, protected);
      ELLE_ATTRIBUTE(bool, checksum, protected);
      ELLE_ATTRIBUTE_R(elle::Version, version);
      ELLE_ATTRIBUTE(elle::reactor::Mutex, lock_write, protected);
      ELLE_ATTRIBUTE(elle::reactor::Mutex, lock_read, protected);
    };

    /*------.
    | Types |
    `------*/

    Serializer::EOF::EOF()
      : elle::Error("end of serializer stream")
    {}

    /*-------------.
    | Construction |
    `-------------*/

    Serializer::Serializer(std::iostream& stream,
                           elle::Version const& version,
                           bool checksum)
      : Super(*elle::reactor::Scheduler::scheduler())
      , _stream(stream)
      , _version(version)
      , _chunk_size(2 << 16)
      , _checksum(checksum)
    {
      if (this->version() >= elle::Version(0, 2, 0))
      {
        ELLE_TRACE("%s: send local version '%s'", *this, this->version())
          elle::serialization::binary::serialize(this->version(), stream);
        stream.flush();
        ELLE_TRACE("%s: read peer version", *this)
        {
          auto peer_version
            = elle::serialization::binary::deserialize<elle::Version>(
              stream);
          ELLE_DEBUG("peer version: %s", peer_version);
          this->_version = std::min(peer_version, this->version());
        }
      }
      ELLE_TRACE("using version: '%s'", this->version());
      this->_impl.reset(
        new Impl(stream, this->_chunk_size, checksum, version));
    }

    Serializer::~Serializer()
    {}

    /*----------.
    | Receiving |
    `----------*/

    elle::Buffer
    Serializer::_read()
    {
      return this->_impl->read();
    }

    /*--------.
    | Sending |
    `--------*/

    void
    Serializer::_write(elle::Buffer const& packet)
    {
      this->_impl->write(packet);
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
