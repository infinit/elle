#include <elle/log.hh>

#include <protocol/Stream.hh>

#ifdef INFINIT_WINDOWS
# include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

namespace infinit
{
  namespace protocol
  {
    /*-------------.
    | Construction |
    `-------------*/

    Stream::Stream(reactor::Scheduler& scheduler)
      : _scheduler(scheduler)
    {}

    Stream::~Stream()
    {}

    /*-----------.
    | Properties |
    `-----------*/

    reactor::Scheduler&
    Stream::scheduler()
    {
      return _scheduler;
    }

    /*--------.
    | Sending |
    `--------*/

    void
    Stream::write(Packet& packet)
    {
      packet.flush();
      this->_write(packet);
    }

    /*------------------.
    | Int serialization |
    `------------------*/

    void
    Stream::_uint32_put(std::ostream& s, uint32_t  i)
    {
      elle::IOStreamClear clearer(s);
      // FIXME: should rethrow the underlying streambuf error.
      if (!s.good())
        throw elle::Exception("stream is not good");
      i = htonl(i);
      s.write(reinterpret_cast<char*>(&i), sizeof(i));
    }

    uint32_t
    Stream::_uint32_get(std::istream& s)
    {
      uint32_t res;
      elle::IOStreamClear clearer(s);
      // FIXME: should rethrow the underlying streambuf error.
      if (!s.good())
        throw elle::Exception("stream is not good");
      s.read(reinterpret_cast<char*>(&res), sizeof(res));
      ELLE_ASSERT_EQ(unsigned(s.gcount()), sizeof(res));
      return ntohl(res);
    }
  }
}
