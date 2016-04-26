#include <elle/log.hh>

#include <protocol/Stream.hh>
#include <protocol/Serializer.hh>

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
    Stream::write(elle::Buffer& packet)
    {
      this->_write(packet);
    }

    /*------------------.
    | Int serialization |
    `------------------*/

    void
    Stream::_uint32_put(elle::Buffer& b, uint32_t i)
    {
      i = htonl(i);
      b.append(&i, 4);
    }
    uint32_t
    Stream::_uint32_get(elle::Buffer& b)
    {
      uint32_t i;
      ELLE_ASSERT_GTE((signed)b.size(), 4);
      i = *(uint32_t*)b.contents();
      memmove(b.mutable_contents(), b.contents()+4, b.size() - 4);
      b.size(b.size()-4);
      return ntohl(i);
    }
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
      if (s.gcount() != sizeof(res))
        throw Serializer::EOF();
      return ntohl(res);
    }
  }
}
