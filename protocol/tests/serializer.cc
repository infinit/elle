#include <protocol/exceptions.hh>
#include <protocol/Serializer.hh>

#include <cryptography/random.hh>

#include <reactor/asio.hh>
#include <reactor/Barrier.hh>
#include <reactor/Scope.hh>
#include <reactor/network/buffer.hh>
#include <reactor/network/exception.hh>
#include <reactor/network/tcp-server.hh>
#include <reactor/scheduler.hh>

#include <elle/test.hh>
#include <elle/cast.hh>
#include <elle/IOStream.hh>
#include <elle/With.hh>

ELLE_LOG_COMPONENT("infinit.protocol.test");

struct Focket // Fake socket.
  : public elle::IOStream
{
  struct StreamBuffer
    : public elle::DynamicStreamBuffer
  {
    typedef elle::DynamicStreamBuffer Super;

    StreamBuffer(elle::Buffer& our,
                  elle::Buffer& their,
                 bool yield_during_writing = true,
                 bool yield_during_reading = true)
      : elle::DynamicStreamBuffer(1024)
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
        reactor::yield();
        auto size_ = std::min(this->_in.size(), 1024lu);
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
        reactor::yield();
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
    : _router(*reactor::Scheduler::scheduler(), "router",
              std::bind(&SocketInstrumentation::_route, this))
    , alice_conf(new Conf)
    , bob_conf(new Conf)
    , _alice_routed(0)
    , _bob_routed(0)
  {
    this->_a_server.listen();
    this->_b_server.listen();
    this->_alice.reset(
      new reactor::network::TCPSocket("127.0.0.1", this->_a_server.port()));
    this->_bob.reset(
      new reactor::network::TCPSocket("127.0.0.1", this->_b_server.port()));
    reactor::yield();
    reactor::yield();
  }

  ~SocketInstrumentation()
  {
    this->_router.terminate_now();
  }

  reactor::network::TCPSocket&
  alice()
  {
    return *this->_alice;
  }

  reactor::network::TCPSocket&
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
    using reactor::network::Buffer;

    std::unique_ptr<reactor::network::Socket> a(this->_a_server.accept());
    std::unique_ptr<reactor::network::Socket> b(this->_b_server.accept());

    elle::With<reactor::Scope>() << [&](reactor::Scope& scope)
    {
      auto route = [&] (reactor::network::Socket* sender,
                        reactor::network::Socket* recipient,
                        size_t& routed,
                        Conf& conf)
        {
          try
          {
            bool relay = true;
            while (relay)
            {
              char buffer[1024];
              ELLE_DEBUG("reading");
              size_t size = sender->read_some(Buffer(buffer, sizeof(buffer)),
                                              1_sec);
              ELLE_DEBUG("read %s", size);
              conf(elle::ConstWeakBuffer(buffer, size));
              if (conf.corrupt_offset >= 0 && \
                  conf.corrupt_offset >= routed && \
                  conf.corrupt_offset < routed + size)
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
          catch (reactor::network::ConnectionClosed const&)
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

  reactor::network::TCPServer _a_server;
  reactor::network::TCPServer _b_server;
  std::unique_ptr<reactor::network::TCPSocket> _alice;
  std::unique_ptr<reactor::network::TCPSocket> _bob;
  reactor::Thread _router;

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

    size_t quota;
    size_t corrupt_offset;
    char corrupt_mask;
  };
public:
  std::unique_ptr<Conf> alice_conf;
  std::unique_ptr<Conf> bob_conf;
private:
  ELLE_ATTRIBUTE_RW(size_t, alice_routed);
  ELLE_ATTRIBUTE_RW(size_t, bob_routed);
};

template <typename SocketProvider>
void
dialog(elle::Version const& version,
       bool checksum,
       std::function<void (SocketProvider&)> const& conf,
       std::function<void (infinit::protocol::Serializer&)> const& a,
       std::function<void (infinit::protocol::Serializer&)> const& b,
       std::function<void (reactor::Thread&, reactor::Thread&, SocketProvider&)> const& f = std::function<void (reactor::Thread&, reactor::Thread&, SocketProvider&)>())
{
  reactor::Scheduler sched;

  reactor::Thread main(
    sched, "main",
    [&] ()
    {
      SocketProvider sockets;
      std::unique_ptr<infinit::protocol::Serializer> alice;
      std::unique_ptr<infinit::protocol::Serializer> bob;
      elle::With<reactor::Scope>() << [&](reactor::Scope& scope)
      {
        scope.run_background(
          "setup alice's serializer",
          [&]
          {
            alice.reset(new infinit::protocol::Serializer(
                          sockets.alice(), version, checksum));
          });
         scope.run_background(
          "setup bob's serializer",
          [&]
          {
            bob.reset(new infinit::protocol::Serializer(
                        sockets.bob(), version, checksum));
          });
        scope.wait();
      };
      conf(sockets);
      elle::With<reactor::Scope>() << [&](reactor::Scope& scope)
      {
        auto& _alice = scope.run_background(
          "alice",
          boost::bind(a, boost::ref(*alice)));
        auto& _bob = scope.run_background(
          "bob",
          boost::bind(b, boost::ref(*bob)));
        if (f)
          scope.run_background(
            "other",
            [&] ()
            {
              f(_alice, _bob, sockets);
            });
        scope.wait();
      };
    });

  sched.run();
}

#define CASES(function)                                                 \
  for (auto const& version: {elle::Version{0, 1, 0}, elle::Version{0, 2, 0}}) \
    for (auto checksum: {true, false})                                  \
      ELLE_TRACE("version: %s, checksum: %s", version, checksum)        \
        function(version, checksum)

static
void
_exchange_packets(elle::Version const& version,
                  bool checksum)
{
  dialog<SocketInstrumentation>(version,
         checksum,
         [] (SocketInstrumentation&) {},
         [] (infinit::protocol::Serializer& s)
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
         [] (infinit::protocol::Serializer& s)
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

static
void
exchange_packets()
{
  CASES(_exchange_packets);
}

static
void
_exchange(elle::Version const& version,
          bool checksum)
{
  std::vector<elle::Buffer> packets={
    infinit::cryptography::random::generate<std::string>(0),
    infinit::cryptography::random::generate<std::string>(1),
    infinit::cryptography::random::generate<std::string>(1000),
    // Crypto is quite slow generating large strings.
    std::string((2 << 12) + 11, 'y'),
    // XXX: Make sure it's always bigger than the chunk size.
    std::string((2 << 21) - 1, 'x')
  };
  dialog<Connector>(version,
         checksum,
         [] (Connector&) {},
         [&] (infinit::protocol::Serializer& s)
         {
           for (auto const& buffer: packets)
             s.write(buffer);
           for (size_t i = 0; i < packets.size(); ++i)
             BOOST_CHECK_EQUAL(s.read(), packets.at(i));
         },
         [&] (infinit::protocol::Serializer& s)
         {
           for (size_t i = 0; i < packets.size(); ++i)
             BOOST_CHECK_EQUAL(s.read(), packets.at(i));
           for (auto const& buffer: packets)
             s.write(buffer);
         });
}

static
void
exchange()
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
    [] (infinit::protocol::Serializer& s)
    {
      elle::Buffer pp;
      elle::IOStream p(pp.ostreambuf());
      char buffer[1025];
      memset(buffer, 0xAA, sizeof(buffer));
      buffer[sizeof(buffer) - 1] = 0;
      p << buffer;
      p.flush();
      s.write(pp);
    },
    [] (infinit::protocol::Serializer& s)
    {
      BOOST_CHECK_THROW(s.read(), reactor::network::ConnectionClosed);
    });
}

static
void
connection_lost_reader()
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
    [] (infinit::protocol::Serializer& s)
    {
      elle::Buffer p("data", 4);
      s.write(p);
      // Getting an error from TCP writes is a bit touchy.
      try
      {
        while (true)
        {
          reactor::sleep(100_ms);
          s.write(p);
        }
      }
      catch (reactor::network::ConnectionClosed const&)
      {
        return;
      }
      BOOST_FAIL("ConnectionClosed exception was expected");
    },
    [] (infinit::protocol::Serializer&)
    {});
}

static
void
connection_lost_sender()
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
    [] (infinit::protocol::Serializer& s)
    {
      elle::Buffer pp;
      elle::IOStream p(pp.ostreambuf());
      char buffer[1025];
      memset(buffer, 0xAA, sizeof(buffer));
      buffer[sizeof(buffer) - 1] = 0;
      p << buffer;
      p.flush();
      s.write(pp);
    },
    [=] (infinit::protocol::Serializer& s)
    {
      if (checksum)
        BOOST_CHECK_THROW(s.read(), infinit::protocol::ChecksumError);
    });
}

static
void
corruption()
{
  CASES(_corruption);
}

static
void
_interruption(elle::Version const& version,
              bool checksum)
{
  reactor::Barrier interrupted("interrupted");
  reactor::Barrier terminated("terminated");
  reactor::Barrier received("received");
  elle::Buffer to_send;
  elle::Buffer to_send2 = elle::Buffer{std::string("ok")};
  dialog<Connector>(
    version,
    checksum,
    [] (Connector& sockets)
    {
    },
    [&] (infinit::protocol::Serializer& s)
    {
      to_send.size(s.chunk_size() * 30);
      try
      {
        ELLE_LOG("write '%f'", to_send)
          s.write(to_send);
      }
      catch (reactor::Terminate const&)
      {
        ELLE_LOG("terminated!!");
        terminated.open();
      }
      ELLE_LOG("write '%f'", to_send2)
        s.write(to_send2);
    },
    [&] (infinit::protocol::Serializer& s)
    {
      // Read.
      auto res = s.read();
      // Sender in version 0.1.0 cannot be interrupted.
      if (version == elle::Version{0, 1, 0})
      {
        received.open();
        BOOST_CHECK_EQUAL(res, to_send);
        // Read a second time.
        res = s.read();
      }
      BOOST_CHECK_EQUAL(res, to_send2);
    },
    [&] (reactor::Thread& alice,
         reactor::Thread& bob,
         Connector& sockets)
    {
      // Wait for buffer to send to be filled.
      while (to_send.size() == 0)
        reactor::yield();
      do
      {
        // Wait for the sending process to begin.
        reactor::yield();
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
        terminated.wait(1_sec);
      if (version == elle::Version{0, 1, 0})
      {
        ELLE_DEBUG("wait for %s", received)
          received.wait(1_sec);
      }
      else if (version == elle::Version{0, 2, 0})
      {
        ELLE_DEBUG("make sure the original packet hasn't been received")
          BOOST_CHECK(!received);
      }
    }
  );
}

static
void
interruption()
{
  CASES(_interruption);
}

ELLE_TEST_SUITE()
{
  auto& suite = boost::unit_test::framework::master_test_suite();
  suite.add(BOOST_TEST_CASE(exchange_packets), 0, valgrind(10, 10));
  suite.add(BOOST_TEST_CASE(exchange), 0, valgrind(10, 10));
  suite.add(BOOST_TEST_CASE(connection_lost_reader), 0, valgrind(3, 10));
  suite.add(BOOST_TEST_CASE(connection_lost_sender), 0, valgrind(3, 10));
  suite.add(BOOST_TEST_CASE(corruption), 0, valgrind(3, 10));
  suite.add(BOOST_TEST_CASE(interruption), 0, valgrind(6, 10));
}
