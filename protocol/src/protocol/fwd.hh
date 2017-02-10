#ifndef ELLE_PROTOCOL_FWD_HH
# define ELLE_PROTOCOL_FWD_HH

namespace elle
{
  namespace protocol
  {
    class Channel;
    class ChanneledStream;
    class BaseRPC;
    template <typename ISerializer, typename OSerializer>
    class RPC;
    class Serializer;
    class Stream;
  }
}

#endif
