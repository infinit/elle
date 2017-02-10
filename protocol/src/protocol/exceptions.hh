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
      typedef elle::Exception Super;
      Error(std::string const& message);
    };

    /// An operation failure because the received data is corrupted.
    class ChecksumError:
      public Error
    {
    public:
      typedef Error Super;
      ChecksumError();
    };

    /// An operation failure because the peer interrupted transmission.
    class InterruptionError:
      public Error
    {
    public:
      typedef Error Super;
      InterruptionError();
    };

    /// A remote RPC could not be called.
    class RPCError:
      public Error
    {
    public:
      typedef Error Super;
      RPCError(std::string const& message);
    };
  }
}

