#include <protocol/exceptions.hh>

namespace elle
{
  namespace protocol
  {
    Error::Error(std::string const& message):
      Super(message)
    {}

    ChecksumError::ChecksumError():
      Super("checksum error")
    {}

    InterruptionError::InterruptionError():
      Super("peer has interrupted sending")
    {}

    RPCError::RPCError(std::string const& message):
      Super(message)
    {}
  }
}
