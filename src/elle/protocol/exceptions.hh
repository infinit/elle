#pragma once

#include <elle/Exception.hh>

namespace elle
{
  namespace protocol
  {
    /// A protocol operation failure.
    class Error:
      public elle::Exception
    {
    public:
      using Super = elle::Exception;
      Error(std::string const& message);
    };

    /// An operation failure because the received data is corrupted.
    class ChecksumError:
      public Error
    {
    public:
      using Super = Error;
      ChecksumError();
    };

    /// An operation failure because the peer interrupted transmission.
    class InterruptionError:
      public Error
    {
    public:
      using Super = Error;
      InterruptionError();
    };

    /// A remote RPC could not be called.
    class RPCError:
      public Error
    {
    public:
      using Super = Error;
      RPCError(std::string const& message);
    };
  }
}

