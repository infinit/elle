#include <elle/protocol/exceptions.hh>
#include <elle/protocol/Serializer.hh>
#include <elle/protocol/ChanneledStream.hh>
#include <elle/protocol/Channel.hh>

#include <elle/cryptography/random.hh>

#include <elle/reactor/asio.hh>
#include <elle/reactor/Barrier.hh>
#include <elle/reactor/Scope.hh>
#include <elle/reactor/network/Error.hh>
#include <elle/reactor/network/tcp-server.hh>
#include <elle/reactor/network/tcp-socket.hh>
#include <elle/reactor/scheduler.hh>

#include <elle/test.hh>
#include <elle/cast.hh>
#include <elle/IOStream.hh>
#include <elle/With.hh>

ELLE_LOG_COMPONENT("elle.protocol.test");

constexpr static elle::Buffer::Size buffer_size = 4096;

struct Focket // Fake socket.
  : public elle::IOStream
{
  struct StreamBuffer
    : public elle::DynamicStreamBuffer
  {
    using Super = elle::DynamicStreamBuffer;

    StreamBuffer(elle::Buffer& our,
                 elle::Buffer& their,
                 bool yield_during_writing = true,
                 bool yield_during_reading = true)
      : elle::DynamicStreamBuffer(buffer_size)
      , _in(our)
      , _bytes_read(0)
      , _yield_during_reading(yield_during_reading)
      , _out(their)
      , _bytes_written(0)
      , _yield_during_writing(yield_during_writing)
    {
      ELLE_TRACE("%s: connect (%s <-> %s)",
                 this, (void*) &this->_in, (void*) &this->_out);
    }

    ~StreamBuffer()
    {
      ELLE_TRACE("%s: destroy", this);
    }

    virtual
    Size
    read(char* buffer, Size size)
    {
      ELLE_TRACE("read %s: %f (%s)",
                 (void*) &this->_in, this->_in, this->_in.size());
      do
      {
        elle::reactor::yield();
        auto size_ = std::min(this->_in.size(), buffer_size);
        ELLE_DEBUG("size: %s", size_);
        if (size_ == 0)
          continue;
        memcpy(buffer, this->_in.mutable_contents(), size_);
        this->_in = elle::Buffer{
          this->_in.mutable_contents() + size_, this->_in.size() - size_
        };
        this->_bytes_read += size_;
        return size_;
      } while (true);
    };

    virtual
    void
    write(char* buffer, Size size)
    {
      this->_bytes_written += size;
      this->_out.append(buffer, size);
      ELLE_TRACE("write %s: %f (%s)",
                 (void*) &this->_out, this->_out, this->_out.size());
      if (this->_yield_during_writing)
        elle::reactor::yield();
    }

  protected:
    // Reading.
    ELLE_ATTRIBUTE(elle::Buffer&, in);
    ELLE_ATTRIBUTE_R(size_t, bytes_read);
    ELLE_ATTRIBUTE_RW(bool, yield_during_reading);
    // Writing.
    ELLE_ATTRIBUTE(elle::Buffer&, out);
    ELLE_ATTRIBUTE_R(size_t, bytes_written);
    ELLE_ATTRIBUTE_RW(bool, yield_during_writing);
  };

  Focket(elle::Buffer& in,
         elle::Buffer& out)
    : elle::IOStream(new StreamBuffer(in, out))
  {
    ELLE_TRACE("%s: create", this);
  }

  ~Focket()
  {
    ELLE_TRACE("%s: destroy", this);
  }

  StreamBuffer*
  stream() const
  {
    return static_cast<StreamBuffer*>(this->_buffer);
  }

  size_t
  read(char* buffer, size_t size)
  {
    return this->stream()->read(buffer, size);
  }

  size_t
  read_some(char* buffer, size_t size)
  {
    ELLE_ABORT("ok");
  }

  void
  write(char* buffer, size_t size)
  {
    return this->stream()->write(buffer, size);
  }

  size_t
  bytes_read() const
  {
    return this->stream()->bytes_read();
  }

  size_t
  bytes_written() const
  {
    return this->stream()->bytes_written();
  }
};

struct Connector
{
  Connector()
    : _alice_buffer()
    , _bob_buffer()
    , _alice(_alice_buffer, _bob_buffer)
    , _bob(_bob_buffer, _alice_buffer)
  {
    ELLE_TRACE("%s: create", this);
  }

  ~Connector()
  {
    ELLE_TRACE("%s: destroy", this);
  }

  Focket&
  alice()
  {
    return this->_alice;
  }

  Focket&
  bob()
  {
    return this->_bob;
  }

  elle::Buffer _alice_buffer, _bob_buffer;
  Focket _alice, _bob;
};

class SocketInstrumentation
{
public:
  struct Conf;

public:
  SocketInstrumentation()
    : _router(*elle::reactor::Scheduler::scheduler(), "router",
              std::bind(&SocketInstrumentation::_route, this))
    , alice_conf(new Conf)
    , bob_conf(new Conf)
    , _alice_routed(0)
    , _bob_routed(0)
  {
    this->_a_server.listen();
    this->_b_server.listen();
    this->_alice.reset(
      new elle::reactor::network::TCPSocket("127.0.0.1", this->_a_server.port()));
    this->_bob.reset(
      new elle::reactor::network::TCPSocket("127.0.0.1", this->_b_server.port()));
    elle::reactor::yield();
    elle::reactor::yield();
  }

  ~SocketInstrumentation()
  {
    this->_router.terminate_now();
  }

  elle::reactor::network::TCPSocket&
  alice()
  {
    return *this->_alice;
  }

  elle::reactor::network::TCPSocket&
  bob()
  {
    return *this->_bob;
  }

  void
  alice_quota(size_t quota)
  {
    this->alice_conf->quota = quota;
  }

  void
  bob_quota(size_t quota)
  {
    this->bob_conf->quota = quota;
  }

  void
  alice_corrupt(size_t offset, char mask = 0xFF)
  {
    this->alice_conf->corrupt_offset = offset;
    this->alice_conf->corrupt_mask = mask;
  }

private:
  void
  _route()
  {
    std::unique_ptr<elle::reactor::network::Socket> a(this->_a_server.accept());
    std::unique_ptr<elle::reactor::network::Socket> b(this->_b_server.accept());

    elle::With<elle::reactor::Scope>() << [&](elle::reactor::Scope& scope)
    {
      auto route = [&] (elle::reactor::network::Socket* sender,
                        elle::reactor::network::Socket* recipient,
                        int64_t& routed,
                        Conf& conf)
        {
          try
          {
            bool relay = true;
            while (relay)
            {
              char buffer[1024];
              ELLE_DEBUG("reading");
              int64_t size =
                sender->read_some(elle::WeakBuffer(buffer),
                                  valgrind(250_ms, 10));
              ELLE_DEBUG("read %s", size);
              conf(elle::ConstWeakBuffer(buffer, size));
              if (conf.corrupt_offset >= 0
                  && conf.corrupt_offset >= routed
                  && conf.corrupt_offset < routed + size)
              {
                int offset = conf.corrupt_offset - routed;
                ELLE_LOG("%s: corrupt byte %s", *this, offset);
                buffer[offset] ^= conf.corrupt_mask;
              }
              routed += size;
              conf(*this);
              if (conf.quota >= 0 && routed > conf.quota)
              {
                ELLE_LOG("%s: quota reached, interrupt", *this);
                size = size - (routed - conf.quota);
                relay = false;
                routed = conf.quota;
              }
              ELLE_DEBUG("%s: route %s bytes", *this, size);
              recipient->write(elle::ConstWeakBuffer(buffer, size));
            }
          }
          catch (elle::reactor::network::ConnectionClosed const&)
          {}
          sender->close();
          recipient->close();
        };
      scope.run_background("A to B",
                           std::bind(route, a.get(), b.get(),
                                     std::ref(this->_alice_routed),
                                     std::ref(*this->alice_conf)));
      scope.run_background("B to A",
                           std::bind(route, b.get(), a.get(),
                                     std::ref(this->_bob_routed),
                                     std::ref(*this->bob_conf)));
      scope.wait();
    };
  }

  elle::reactor::network::TCPServer _a_server;
  elle::reactor::network::TCPServer _b_server;
  std::unique_ptr<elle::reactor::network::TCPSocket> _alice;
  std::unique_ptr<elle::reactor::network::TCPSocket> _bob;
  elle::reactor::Thread _router;

public:
  struct Conf
  {
    Conf():
      quota(-1),
      corrupt_offset(-1),
      corrupt_mask(0)
    {}

    virtual
    void
    operator ()(elle::ConstWeakBuffer const& data)
    {
    }

    virtual
    void
    operator ()(SocketInstrumentation const& sockets)
    {
    }

    int64_t quota;
    int64_t corrupt_offset;
    char corrupt_mask;
  };
public:
  std::unique_ptr<Conf> alice_conf;
  std::unique_ptr<Conf> bob_conf;
private:
  ELLE_ATTRIBUTE_RW(int64_t, alice_routed);
  ELLE_ATTRIBUTE_RW(int64_t, bob_routed);
};

template <typename SocketProvider>
void
dialog(elle::Version const& version,
       bool checksum,
       std::function<void (SocketProvider&)> const& conf,
       std::function<void (elle::protocol::Serializer&)> const& a,
       std::function<void (elle::protocol::Serializer&)> const& b,
       std::function<void (elle::reactor::Thread&, elle::reactor::Thread&,
                           SocketProvider&)> const& f = {})
{
  SocketProvider sockets;
  std::unique_ptr<elle::protocol::Serializer> alice;
  std::unique_ptr<elle::protocol::Serializer> bob;
  elle::With<elle::reactor::Scope>() << [&](elle::reactor::Scope& scope)
  {
    scope.run_background(
      "setup alice's serializer",
      [&]
      {
        alice.reset(new elle::protocol::Serializer(
                      sockets.alice(), version, checksum));
      });
    scope.run_background(
      "setup bob's serializer",
      [&]
      {
        bob.reset(new elle::protocol::Serializer(
                    sockets.bob(), version, checksum));
      });
    scope.wait();
  };
  conf(sockets);
  elle::With<elle::reactor::Scope>() << [&](elle::reactor::Scope& scope)
  {
    auto& _alice = scope.run_background(
      "alice",
      std::bind(a, std::ref(*alice)));
    auto& _bob = scope.run_background(
      "bob",
      std::bind(b, std::ref(*bob)));
    if (f)
      scope.run_background(
        "other",
        [&] ()
        {
          f(_alice, _bob, sockets);
        });
    scope.wait();
  };
}

#define CASES(function)                                                 \
  for (auto const& version: {elle::Version{0, 1, 0},                    \
                             elle::Version{0, 2, 0}})                   \
    for (auto checksum: {true, false})                                  \
      ELLE_LOG("case: version = %s, checksum = %s", version, checksum)  \
        function(version, checksum)                                     \

static
void
_exchange_packets(elle::Version const& version,
                  bool checksum)
{
  dialog<SocketInstrumentation>(version,
         checksum,
         [] (SocketInstrumentation&) {},
         [] (elle::protocol::Serializer& s)
         {
           {
             elle::Buffer p("some data 42", strlen("some data 42"));
             s.write(p);
           }
           {
             elle::Buffer pp = s.read();
             elle::IOStream p(pp.istreambuf());
             std::string res;
             p >> res;
             BOOST_CHECK_EQUAL(res, "goody");
             BOOST_CHECK(!(p >> res));
           }
         },
         [] (elle::protocol::Serializer& s)
         {
           {
             elle::Buffer pp = s.read();
             elle::IOStream p(pp.istreambuf());
             std::string some, data;
             int i;
             p >> some;
             BOOST_CHECK_EQUAL(some, "some");
             p >> data;
             BOOST_CHECK_EQUAL(data, "data");
             p >> i;
             BOOST_CHECK_EQUAL(i, 42);
             BOOST_CHECK(!(p >> data));
           }
           {
             elle::Buffer p("goody", 5);
             s.write(p);
           }
         });
}

ELLE_TEST_SCHEDULED(exchange_packets)
{
  CASES(_exchange_packets);
}

static
void
_exchange(elle::Version const& version,
          bool checksum)
{
  std::vector<elle::Buffer> packets={
    elle::cryptography::random::generate<elle::Buffer>(0),
    elle::cryptography::random::generate<elle::Buffer>(1),
    elle::cryptography::random::generate<elle::Buffer>(1000),
    // Crypto is quite slow generating large buffers
    std::string((2 << 18) + 11, 'y'),
    // // XXX: Make sure it's always bigger than the chunk size.
    std::string((2 << 21) - 1, 'x')
  };
  dialog<Connector>(version,
         checksum,
         [] (Connector&) {},
         [&] (elle::protocol::Serializer& s)
         {
           for (auto const& buffer: packets)
             s.write(buffer);
           for (size_t i = 0; i < packets.size(); ++i)
             BOOST_CHECK_EQUAL(s.read(), packets.at(i));
         },
         [&] (elle::protocol::Serializer& s)
         {
           for (size_t i = 0; i < packets.size(); ++i)
             BOOST_CHECK_EQUAL(s.read(), packets.at(i));
           for (auto const& buffer: packets)
             s.write(buffer);
         });
}

ELLE_TEST_SCHEDULED(exchange)
{
  CASES(_exchange);
}

static
void
_connection_lost_reader(elle::Version const& version,
                        bool checksum)
{
  dialog<SocketInstrumentation>(
    version,
    checksum,
    [] (SocketInstrumentation& sockets)
    {
      // Reset 'routed' to ignore version exchange.
      sockets.alice_routed(0);
      sockets.bob_routed(0);
      sockets.alice_quota(96);
    },
    [] (elle::protocol::Serializer& s)
    {
      elle::Buffer pp;
      elle::IOStream p(pp.ostreambuf());
      char buffer[1025];
      memset(buffer, 0xAA, sizeof buffer);
      buffer[sizeof buffer - 1] = 0;
      p << buffer;
      p.flush();
      s.write(pp);
    },
    [] (elle::protocol::Serializer& s)
    {
      BOOST_CHECK_THROW(s.read(), elle::reactor::network::ConnectionClosed);
    });
}

ELLE_TEST_SCHEDULED(connection_lost_reader)
{
  CASES(_connection_lost_reader);
}

static
void
_connection_lost_sender(elle::Version const& version,
                        bool checksum)
{
  dialog<SocketInstrumentation>(
    version,
    checksum,
    [] (SocketInstrumentation& sockets)
    {
      // Reset 'routed' to ignore version exchange.
      sockets.alice_routed(0);
      sockets.bob_routed(0);
      sockets.alice_quota(4);
    },
    [] (elle::protocol::Serializer& s)
    {
      elle::Buffer p("data", 4);
      s.write(p);
      // Getting an error from TCP writes is a bit touchy.
      try
      {
        while (true)
        {
          elle::reactor::sleep(valgrind(10_ms, 10));
          s.write(p);
        }
      }
      catch (elle::reactor::network::ConnectionClosed const&)
      {
        return;
      }
      BOOST_FAIL("ConnectionClosed exception was expected");
    },
    [] (elle::protocol::Serializer&)
    {});
}

ELLE_TEST_SCHEDULED(connection_lost_sender)
{
  CASES(_connection_lost_sender);
}

static
void
_corruption(elle::Version const& version,
            bool checksum)
{
  dialog<SocketInstrumentation>(
    version,
    checksum,
    [] (SocketInstrumentation& sockets)
    {
      // Reset 'routed' to ignore version exchange.
      sockets.alice_routed(0);
      sockets.bob_routed(0);
      sockets.alice_corrupt(1024);
    },
    [] (elle::protocol::Serializer& s)
    {
      elle::Buffer pp;
      elle::IOStream p(pp.ostreambuf());
      char buffer[1025];
      memset(buffer, 0xAA, sizeof buffer);
      buffer[sizeof buffer - 1] = 0;
      p << buffer;
      p.flush();
      s.write(pp);
    },
    [=] (elle::protocol::Serializer& s)
    {
      if (checksum)
        BOOST_CHECK_THROW(s.read(), elle::protocol::ChecksumError);
    });
}

ELLE_TEST_SCHEDULED(corruption)
{
  CASES(_corruption);
}

static
void
_interruption(elle::Version const& version,
              bool checksum)
{
  // A packet of size 1 means interruption will happen while sending the last
  // (and unique) block. This one should go through.
  for (auto number: std::vector<int>({30, 10, 1}))
  {
    elle::reactor::Barrier interrupted("interrupted");
    elle::reactor::Barrier terminated("terminated");
    elle::reactor::Barrier received("received");
    elle::Buffer to_send;
    elle::Buffer to_send2 = elle::Buffer{std::string("ok")};
    dialog<Connector>(
      version,
      checksum,
      [] (Connector& sockets)
      {
      },
      [&] (elle::protocol::Serializer& s)
      {
        to_send = elle::cryptography::random::generate<elle::Buffer>(
          s.chunk_size() * number);
        try
        {
          ELLE_TRACE("write '%f'", to_send)
            s.write(to_send);
        }
        catch (elle::reactor::Terminate const&)
        {
          ELLE_LOG("terminated!!")
            terminated.open();
        }
        ELLE_TRACE("write '%f'", to_send2)
          s.write(to_send2);
      },
      [&] (elle::protocol::Serializer& s)
      {
        // Read.
        auto res = s.read();
        // Sender in version 0.1.0 cannot be interrupted.
        if (version == elle::Version{0, 1, 0} || number == 1)
        {
          received.open();
          BOOST_CHECK_EQUAL(res, to_send);
          // Read a second time.
          res = s.read();
        }
        BOOST_CHECK_EQUAL(res, to_send2);
      },
      [&] (elle::reactor::Thread& alice,
           elle::reactor::Thread& bob,
           Connector& sockets)
      {
        // Wait for buffer to send to be filled.
        while (to_send.size() == 0)
          elle::reactor::yield();
        do
        {
          // Wait for the sending process to begin.
          elle::reactor::yield();
          ELLE_DEBUG("bytes_written: %s", sockets.alice().bytes_written());
          if (sockets.alice().bytes_written() > (to_send.size() / 5))
          {
            ELLE_LOG("terminate after sending %s bytes (over %s bytes)",
                     sockets.alice().bytes_written(), to_send.size());
            alice.terminate_now();
            break;
          }
        } while (true);
        // Version one will go throught even if a termination has been required.
        ELLE_DEBUG("wait for %s", terminated)
          terminated.wait(valgrind(250_ms, 10));
        if (version == elle::Version{0, 1, 0})
        {
          ELLE_DEBUG("wait for %s", received)
            received.wait(valgrind(250_ms, 10));
        }
        else if (version >= elle::Version{0, 2, 0})
        {
          ELLE_DEBUG("make sure the original packet hasn't been received")
            if (number != 1)
              BOOST_CHECK(!received);
        }
      }
    );
  }
}

ELLE_TEST_SCHEDULED(interruption)
{
  CASES(_interruption);
}

static
void
_termination(elle::Version const& version,
            bool checksum)
{
  elle::reactor::Thread::unique_ptr t;
  elle::reactor::Barrier tready;
  dialog<SocketInstrumentation>(
    version,
    checksum,
    [] (SocketInstrumentation& sockets)
    {
    },
    [&] (elle::protocol::Serializer& s)
    {
      t.reset(new elle::reactor::Thread("reader", [&] {
          try
          {
            ELLE_TRACE("read from thread");
            tready.open();
            s.read();
            BOOST_CHECK(false);
          }
          catch (elle::reactor::Terminate const& t)
          {
            ELLE_TRACE("terminating first reader");
            throw;
          }
      }));
      elle::reactor::wait(*t);
      ELLE_TRACE("second read")
      {
        auto buf = s.read();
        ELLE_TRACE("reader done");
        BOOST_CHECK_EQUAL(buf.string(), "foobar");
      }
    },
    [&] (elle::protocol::Serializer& s)
    {
      ELLE_TRACE("waitinng for thread")
        elle::reactor::wait(tready);
      auto& backend = s.stream();
      if (version >= elle::Version(0, 3, 0))
      {
        char c = 0;
        backend.write(&c, 1);
      }
      elle::protocol::Stream::uint32_put(backend, 6, version);
      backend.write("foo", 3);
      backend.flush();
      elle::reactor::sleep(valgrind(10_ms, 10));
      ELLE_TRACE("killing thread")
        t->terminate();
      elle::reactor::sleep(valgrind(10_ms, 10));
      backend.write("baz", 3);
      if (version >= elle::Version(0, 3, 0))
      {
        char c = 0;
        backend.write(&c, 1);
      }
      elle::protocol::Stream::uint32_put(backend, 6, version);
      backend.write("foobar", 6);
      backend.flush();
      ELLE_TRACE("writer done");
    });
}

ELLE_TEST_SCHEDULED(termination)
{
  _termination(elle::Version{0, 1, 0}, false);
  _termination(elle::Version{0, 2, 0}, false);
  _termination(elle::Version{0, 3, 0}, false);
}

ELLE_TEST_SCHEDULED(eof)
{
  static std::string const data(
    "If the dream of humanity comes true, "
    "will there be anyone around to witness it ?");
  std::string packet;
  std::string valid_packet;
  {
    std::stringstream output;
    {
      elle::protocol::Serializer s(output);
      elle::Buffer p(data);
      s.write(p);
    }
    packet = output.str();
    valid_packet = packet;
  }
  packet = packet.substr(0, packet.length() - 1);
  {
    std::stringstream input(packet);
    {
      elle::protocol::Serializer s(input);
      BOOST_CHECK_THROW(s.read(), elle::protocol::Serializer::EOF);
    }
  }
  {
    std::stringstream input;
    elle::protocol::Serializer s(input);
    BOOST_CHECK_THROW(s.read(), elle::protocol::Serializer::EOF);
  }
  {
   std::stringstream input(valid_packet);
   elle::protocol::Serializer s(input);
   BOOST_CHECK_NO_THROW(s.read());
   BOOST_CHECK_THROW(s.read(), elle::protocol::Serializer::EOF);
  }
}

ELLE_TEST_SCHEDULED(message)
{
  for (auto version: {elle::Version{0, 2, 0}, elle::Version{0, 3, 0}})
  {
    uint32_t chunk_size = 0;
    dialog<Connector>(
      version,
      false,
      [] (Connector&)
      {
      },
      [&] (elle::protocol::Serializer& s)
      {
        chunk_size = s.chunk_size();
        auto buf = s.read();
        ELLE_LOG("recieved %f (size %s)", buf, buf.size());
        elle::Buffer expected{std::string(chunk_size * 2, 'y')};
        BOOST_CHECK_EQUAL(buf.size(), expected.size());
        BOOST_CHECK_EQUAL(buf, expected);
      },
      [&] (elle::protocol::Serializer& s)
      {
      },
      [&] (elle::reactor::Thread&, elle::reactor::Thread&, Connector& connector)
      {
        // XXX: This could be better... Once we have the version that send
        // messages, re-write that test.
        while (chunk_size == 0)
          elle::reactor::yield();
        auto size = 2 * chunk_size;
        auto chunk = elle::Buffer{std::string(chunk_size, 'y')};
        auto keep_going = [&] () {
          unsigned char keep_going = 0;
          connector._alice_buffer.append(&keep_going, 1);
        };
        if (version >= elle::Version(0, 3, 0))
          keep_going();
        // Send the size.
        {
          elle::protocol::Stream::uint32_put(
            connector._alice_buffer, size, version);
        }
        // Write first part.
        connector._alice_buffer.append(chunk.mutable_contents(), chunk.size());
        // Put a message in the middle of the stream.
        {
          unsigned char message_control = 2;
          std::string message = "this should be ignored";
          connector._alice_buffer.append(&message_control, 1);
          elle::protocol::Stream::uint32_put(
            connector._alice_buffer, message.length(), version);
          // connector._alice_buffer.append(&size, 4);
          connector._alice_buffer.append(message.c_str(), message.length());
        }
        // Keep going.
        keep_going();
        // Write the second part.
        connector._alice_buffer.append(chunk.mutable_contents(), chunk.size());
      });
  }
}

ELLE_TEST_SCHEDULED(interruption2)
{
  // Check that terminating a Channel.read() call does not lose an unrelated
  // packet.
  namespace ip = elle::protocol;

  for (auto version: {elle::Version{0, 2, 0}, elle::Version{0, 3, 0}})
  {
    ELLE_LOG("test version %s", version);
    elle::reactor::Barrier pinger_block;
    dialog<Connector>(
      version,
      false,
      [] (Connector&)
      {
      },
      [&] (elle::protocol::Serializer& s)
      { // echo server, sending partial result until pinger_block is opened.
        ip::ChanneledStream stream(s);
        while (true)
        {
          ELLE_TRACE("accept");
          ip::Channel c = stream.accept();
          auto buf = c.read();
          // Forge a packet in `message`.
          elle::Buffer message;
          // the channel id.
          elle::protocol::Stream::uint32_put(message, c.id(), version);
          // the content.
          {
            elle::IOStream m(message.ostreambuf());
            m.write((const char*)buf.contents(), buf.size());
          }
          // Send control char
          if (version >= elle::Version(0, 3, 0))
          {
            char c = 0;
            s.stream().write(&c, 1);
          }
          // Send the size of `message`.
          elle::protocol::Stream::uint32_put(s.stream(), message.size(), version);
          {
            // Send `message`, but lacking the end.
            s.stream().write((const char*)message.contents(), message.size() - 1);
          }
          ELLE_TRACE("partial write, wait for signal");
          elle::reactor::wait(pinger_block);
          ELLE_TRACE("finish write");
          pinger_block.close();
          // Finish sending `message`.
          s.stream().write((const char*)message.contents() + message.size()-1, 1);
          s.stream().flush();
          ELLE_TRACE("packet transmited");
        }
      },
      [&] (elle::protocol::Serializer& s)
      {
        ip::ChanneledStream stream(s);
        ip::Channel c(stream);
        c.write(elle::Buffer("foo"));
        elle::reactor::Barrier b;
        b.close();
        elle::reactor::Thread t("c2read", [&] {
            ip::Channel c2(stream);
            ELLE_TRACE("open b and read");
            b.open();
            auto buf = c2.read();
            elle::unreachable();
          });
        ELLE_TRACE("Wait b");
        b.wait();
        pinger_block.open();
        ELLE_TRACE("kill t");
        t.terminate_now();
        ELLE_TRACE("t is gone");
        ip::Channel c2(stream);
        c2.write(elle::Buffer("bar"));
        pinger_block.open();
        ELLE_TRACE("read on %s", c.id());
        auto buf = c.read();
        BOOST_CHECK_EQUAL(buf.string(), "foo");

        // just read some more to check we're still on
        ip::Channel c3(stream);
        c3.write(elle::Buffer("baz"));
        while (pinger_block.opened())
          elle::reactor::yield();
        pinger_block.open();
        ELLE_TRACE("read c3");
        buf = c3.read();
        BOOST_CHECK_EQUAL(buf.string(), "baz");

        // check killing reader thread before any data is read
        b.close();
        elle::reactor::Thread t2("read nothing", [&] {
            ip::Channel c2(stream);
            ELLE_TRACE("open b and read");
            b.open();
            auto buf = c2.read();
            elle::unreachable();
          });
        ELLE_TRACE("Wait b");
        b.wait();
        t2.terminate_now();
        // check everything still works
        ip::Channel c4(stream);
        c4.write(elle::Buffer("maz"));
        while (pinger_block.opened())
          elle::reactor::yield();
        pinger_block.open();
        ELLE_TRACE("read c4");
        buf = c4.read();
        BOOST_CHECK_EQUAL(buf.string(), "maz");
      },
      [&] (elle::reactor::Thread& t1, elle::reactor::Thread& t2, Connector& connector)
      {
        elle::reactor::wait(t2);
        t1.terminate_now();
      });
  }
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(exchange_packets), 0, valgrind(10, 10));
  suite.add(BOOST_TEST_CASE(exchange), 0, valgrind(20, 10));
  suite.add(BOOST_TEST_CASE(connection_lost_reader), 0, valgrind(3, 10));
  suite.add(BOOST_TEST_CASE(connection_lost_sender), 0, valgrind(3, 10));
  suite.add(BOOST_TEST_CASE(corruption), 0, valgrind(3, 10));
  suite.add(BOOST_TEST_CASE(interruption), 0, valgrind(6, 15));
  suite.add(BOOST_TEST_CASE(interruption2), 0, valgrind(3, 10));
  suite.add(BOOST_TEST_CASE(termination), 0, valgrind(3, 10));
  suite.add(BOOST_TEST_CASE(eof), 0, valgrind(3));
  suite.add(BOOST_TEST_CASE(message), 0, valgrind(3));
}
