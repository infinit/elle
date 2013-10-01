#include <protocol/exceptions.hh>

namespace infinit
{
  namespace protocol
  {
    Error::Error(std::string const& message):
      Super(message)
    {}

    ChecksumError::ChecksumError():
      Super("checksum error")
    {}
  }
}
