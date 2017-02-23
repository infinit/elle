#include <elle/log.hh>

#include <elle/protocol/Stream.hh>
#include <elle/protocol/Serializer.hh>
#include <elle/protocol/exceptions.hh>

#include <elle/reactor/scheduler.hh>

#ifdef INFINIT_WINDOWS
# include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include <elle/serialization/binary.hh>

namespace elle
{
  namespace protocol
  {
    using namespace elle::serialization::binary;

    /*-------------.
    | Construction |
    `-------------*/

    Stream::Stream(elle::reactor::Scheduler& scheduler)
      : _scheduler(scheduler)
    {}

    Stream::Stream()
      : Stream(*elle::reactor::Scheduler::scheduler())
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
    Stream::uint32_put(elle::Buffer& b, uint32_t i, elle::Version const& v)
    {
      if (v >= elle::Version(0, 3, 0))
      {
        elle::IOStream output(b.ostreambuf());
        SerializerOut::serialize_number(output, i);
      }
      else
      {
        i = htonl(i);
        b.append(&i, 4);
      }
    }

    uint32_t
    Stream::uint32_get(elle::Buffer& b, elle::Version const& v)
    {
      if (v >= elle::Version(0, 3, 0))
      {
        elle::IOStream input(b.istreambuf());
        int64_t res;
        b.pop_front(SerializerIn::serialize_number(input, res));
        return (uint32_t) res;
      }
      else
      {
        uint32_t i;
        ELLE_ASSERT_GTE((signed)b.size(), 4);
        i = *(uint32_t*)b.contents();
        b.pop_front(4);
        return ntohl(i);
      }
    }

    void
    Stream::uint32_put(std::ostream& s, uint32_t i, elle::Version const& v)
    {
      if (v >= elle::Version(0, 3, 0))
      {
        SerializerOut::serialize_number(s, i);
      }
      else
      {
        elle::IOStreamClear clearer(s);
        // FIXME: should rethrow the underlying streambuf error.
        if (!s.good())
          throw elle::Exception("stream is not good");
        i = htonl(i);
        s.write(reinterpret_cast<char*>(&i), sizeof(i));
      }
    }

    uint32_t
    Stream::uint32_get(std::istream& s, elle::Version const& v)
    {
      if (v >= elle::Version(0, 3, 0))
      {
        int64_t res = 0;
        SerializerIn::serialize_number(s, res);
        if (res < 0 || std::numeric_limits<uint32_t>::max() < res)
          throw Error(elle::sprintf("unexpected uint32_t: %s", res));
        return (uint32_t) res;
      }
      else
      {
        uint32_t res = 0;
        elle::IOStreamClear clearer(s);
        // FIXME: should rethrow the underlying streambuf error.
        if (!s.good())
          throw elle::Exception("stream is not good");
        s.read(reinterpret_cast<char*>(&res), sizeof(res));
        if (s.gcount() != signed(sizeof(res)))
          throw Serializer::EOF();
        return ntohl(res);
      }
    }
  }
}
