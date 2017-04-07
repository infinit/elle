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
    read(std::istream& stream,
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
        ELLE_DUMP_SCOPE("read from %s", nread);
        elle::IOStreamClear clearer(stream);
        nread += std::readsome(stream, where, size - nread);
        if (stream.eof())
          throw Serializer::EOF();
      }
    }

    static
    elle::Buffer
    read(std::istream& stream,
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
    write(std::ostream& stream,
          elle::Version const& version,
          elle::Buffer const& content,
          bool write_size = true,
          elle::Buffer::Size offset = 0,
          boost::optional<elle::Buffer::Size> size = boost::none)
    {
      elle::Buffer::Size to_send = size ? size.get() : content.size();
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
      ping = 3,
      pong = 4,
      max = pong,
    };

    static
    void
    ignore_message(std::istream& stream, elle::Version const& version)
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
           elle::Version const& version,
           boost::optional<std::chrono::milliseconds> ping_period,
           boost::optional<std::chrono::milliseconds> ping_timeout)
        : _broken(false)
        , _scheduler(reactor::scheduler())
        , _pings(0)
        , _pongs(0)
        , _ping_period(std::move(ping_period))
        , _ping_delay(std::move(ping_timeout))
        , _pinger(this->_scheduler.io_service())
        , _pinger_handler(
          [this] (boost::system::error_code const& error)
          {
            if (error == boost::system::errc::operation_canceled)
              return;
            else if (error)
              ELLE_ABORT("%s: unexpected timer error: %s", this, error);
            this->ping();
            this->_pinger.expires_from_now(
              boost::posix_time::milliseconds(this->_ping_period->count()));
            this->_pinger.async_wait(this->_pinger_handler);
          })
        , _stream(stream)
        , _chunk_size(chunk_size)
        , _checksum(checksum)
        , _version(version)
        , _lock_write()
        , _lock_read()
      {
        if (bool(this->_ping_period) != bool(this->_ping_delay))
          elle::err("specify either both ping period and timeout or neither");
        if (this->_ping_period && this->version() >= elle::Version(0, 3, 0))
          this->_pinger_handler({});
      }

      virtual
      ~Impl() = default;

    public:
      elle::Buffer
      read()
      {
        while (true)
        {
          if (this->_broken)
            elle::err("stream is broken by a previous interrupted read");
          try
          {
            elle::reactor::Lock lock(this->_lock_read);
            if (this->_broken)
              elle::err("stream is broken by a previous interrupted read");
            elle::IOStreamClear clearer(this->_stream);
            return this->_read();
          }
          catch (InterruptionError const&)
          {}
        }
      }

      /// Whether the stream is broken by a previous interrupted read.
      ELLE_ATTRIBUTE_R(bool, broken);

      elle::Buffer
      _read()
      {
        if (this->version() >= elle::Version(0, 3, 0))
          while (!this->read_control())
            ;
        try
        {
        // return elle::With<elle::reactor::Thread::NonInterruptible>() << [&]
        // {
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
              uint32_t total_size =
                Serializer::Super::uint32_get(this->_stream, this->version());
              ELLE_DEBUG("packet size: %s", total_size);
              elle::Buffer packet(static_cast<std::size_t>(total_size));
              elle::Buffer::Size offset = 0;
              while (true)
              {
                uint32_t size =
                  std::min(total_size - offset, this->_chunk_size);
                ELLE_DEBUG("read chunk of size %s", size);
                elle::protocol::read(this->_stream, packet, size, offset);
                offset += size;
                ELLE_ASSERT_LTE(offset, total_size);
                if (offset >= total_size)
                  break;
                if (!this->read_control())
                  throw InterruptionError();
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
        catch (InterruptionError const&)
        {
          throw;
        }
        catch (...)
        {
          ELLE_TRACE("read interrupted, switching to broken state: %s",
                     elle::exception_string());
          this->_broken = true;
          throw;
        }
      }

      void
      write(elle::Buffer const& packet)
      {
        elle::reactor::Lock lock(this->_lock_write);
        elle::IOStreamClear clearer(this->_stream);
        this->_write(packet);
      }

      void
      write_control(Control control)
      {
        if (control != Control::pong && control != Control::ping)
          this->write_pings_pongs(false);
        ELLE_DEBUG_SCOPE("send control %s", (int) control);
        char c = static_cast<char>(control);
        this->_stream.write(&c, 1);
      }

      void
      write_pings_pongs(bool flush)
      {
        while (this->_pongs || this->_pings)
        {
          while (this->_pongs)
          {
            --this->_pongs;
            this->write_control(Control::pong);
          }
          while (this->_pings)
          {
            --this->_pings;
            this->write_control(Control::ping);
          }
         if (flush)
           this->_stream.flush();
        }
      }

      bool
      read_control()
      {
        while (true)
        {
          ELLE_DUMP_SCOPE("read control");
          if (this->_stream.peek() == std::iostream::traits_type::eof())
            throw Serializer::EOF();
          char control = static_cast<char>(Control::max + 1);
          this->_stream.read(&control, 1);
          if (control > Control::max)
          {
            ELLE_ERR("%s: invalid control byte: 0x%x",
                     this, static_cast<int>(control));
            elle::err<protocol::Error>(
              "invalid control byte: 0x%x", static_cast<int>(control));
          }
          else
            ELLE_DUMP("control: '%x'", (int) control);
          switch (static_cast<Control>(control))
          {
            case Control::keep_going:
              return true;
            case Control::interrupt:
              return false;
            case Control::message:
              ignore_message(this->_stream, this->version());
              break;
            case Control::ping:
              this->pinged();
              break;
            case Control::pong:
              if (this->_ping_timers.empty())
                ELLE_ERR("%s: unexpected pong", this);
              else
                this->_ping_timers.pop_front();
              break;
          }
        }
      }

      void
      ping()
      {
        ++this->_pings;
        if (!this->_lock_write.locked())
          this->write_pings_pongs(true);
        this->_ping_timers.emplace_back(this->_scheduler.io_service());
        this->_ping_timers.back().expires_from_now(
          boost::posix_time::milliseconds(this->_ping_delay->count()));
        this->_ping_timers.back().async_wait(
          [this] (boost::system::error_code const& error)
          {
            if (!error)
              this->_ping_timeout();
          });
      }

      void
      pinged()
      {
        ++this->_pongs;
        if (!this->_lock_write.locked())
        {
          elle::reactor::Lock lock(this->_lock_write);
          this->write_pings_pongs(true);
        }
      }

      ELLE_ATTRIBUTE(elle::reactor::Scheduler&, scheduler);
      ELLE_ATTRIBUTE(int, pings);
      ELLE_ATTRIBUTE(int, pongs);
      ELLE_ATTRIBUTE(boost::optional<std::chrono::milliseconds>, ping_period);
      ELLE_ATTRIBUTE(boost::optional<std::chrono::milliseconds>, ping_delay);
      ELLE_ATTRIBUTE(boost::asio::deadline_timer, pinger);
      ELLE_ATTRIBUTE(std::function<void (boost::system::error_code const&)>,
                     pinger_handler);
      ELLE_ATTRIBUTE(std::list<boost::asio::deadline_timer>, ping_timers);
      ELLE_ATTRIBUTE_RX(boost::signals2::signal<void ()>, ping_timeout);

      void
      _write(elle::Buffer const& packet)
      {
        if (this->version() >= elle::Version(0, 3, 0))
          this->write_control(Control::keep_going);
        if (this->_checksum)
        {
          // Compute and send checksum.
          auto hash = compute_checksum(packet);
          elle::With<elle::reactor::Thread::NonInterruptible>() << [&]
          {
            ELLE_DEBUG("send checksum: 0x%x", hash)
              elle::protocol::write(this->_stream, this->version(), hash);
          };
        }
        if (this->version() >= elle::Version(0, 2, 0))
        {
          elle::Buffer::Size offset = 0;
          try
          {
            auto send = [&]
              {
                auto to_send = std::min(this->_chunk_size, packet.size() - offset);
                ELLE_DEBUG_SCOPE("send %s bytes of data at offset %s",
                                 to_send, offset);
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
              elle::With<elle::reactor::Thread::NonInterruptible>() << [&]
              {
                this->write_control(Control::keep_going);
                send();
              };
              this->write_pings_pongs(true);
            }
          }
          catch (elle::reactor::Terminate const&)
          {
            if (offset < packet.size())
            {
              ELLE_DEBUG("interrupted after sending %s bytes over %s",
                         offset, packet.size());
              this->write_control(Control::interrupt);
              this->write_pings_pongs(true);
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

    Serializer::Serializer(
      std::iostream& stream,
      elle::Version const& version,
      bool checksum,
      boost::optional<std::chrono::milliseconds> ping_period,
      boost::optional<std::chrono::milliseconds> ping_timeout,
      elle::Buffer::Size chunk_size)
      : Super(*elle::reactor::Scheduler::scheduler())
      , _stream(stream)
      , _version(version)
      , _chunk_size(chunk_size)
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
        new Impl(stream, this->_chunk_size, checksum, version,
                 std::move(ping_period), std::move(ping_timeout)));
      this->_impl->ping_timeout().connect(this->_ping_timeout);
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
