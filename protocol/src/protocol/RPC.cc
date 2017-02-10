#include <reactor/exception.hh>

#include <protocol/RPC.hh>

namespace elle
{
  namespace protocol
  {
    BaseRPC::BaseRPC(ChanneledStream& channels)
      : _channels(channels)
      , _id(0)
    {}
  }
}
