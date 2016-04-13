#include <elle/log.hh>

#include <protocol/Stream.hh>

#include <reactor/scheduler.hh>

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

    Stream::Stream()
      : Stream(*reactor::Scheduler::scheduler())
    {}

    Stream::~Stream()
    {}

    /*--------.
    | Sending |
    `--------*/

    void
    Stream::write(elle::Buffer const& packet)
    {
      this->_write(packet);
    }

    /*------------------.
    | Int serialization |
    `------------------*/

    void
    Stream::uint32_put(elle::Buffer& b, uint32_t i)
    {
      i = htonl(i);
      b.append(&i, 4);
    }

    uint32_t
    Stream::uint32_get(elle::Buffer& b)
    {
      uint32_t i;
      ELLE_ASSERT_GTE((signed)b.size(), 4);
      i = *(uint32_t*)b.contents();
      memmove(b.mutable_contents(), b.contents() + 4, b.size() - 4);
      b.size(b.size() - 4);
      return ntohl(i);
    }

    void
    Stream::uint32_put(std::ostream& s, uint32_t  i)
    {
      elle::IOStreamClear clearer(s);
      // FIXME: should rethrow the underlying streambuf error.
      if (!s.good())
        throw elle::Exception("stream is not good");
      i = htonl(i);
      s.write(reinterpret_cast<char*>(&i), sizeof(i));
    }

    uint32_t
    Stream::uint32_get(std::istream& s)
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
