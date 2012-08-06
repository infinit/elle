#include <arpa/inet.h>

#include <elle/log.hh>

#include <protocol/Stream.hh>

ELLE_LOG_TRACE_COMPONENT("Infinit.Protocol");

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

    /*------------------.
    | Int serialization |
    `------------------*/

    void
    Stream::_uint32_put(std::ostream& s, uint32_t  i)
    {
      i = htonl(i);
      s.write(reinterpret_cast<char*>(&i), sizeof(i));
    }

    uint32_t
    Stream::_uint32_get(std::istream& s)
    {
      uint32_t res;
      s.read(reinterpret_cast<char*>(&res), sizeof(res));
      return ntohl(res);
    }
  }
}
