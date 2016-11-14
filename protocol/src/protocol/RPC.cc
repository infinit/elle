#include <reactor/exception.hh>

#include <protocol/RPC.hh>

namespace infinit
{
  namespace protocol
  {
    BaseRPC::BaseRPC(ChanneledStream& channels)
      : _channels(channels)
      , _id(0)
    {}
  }
}
