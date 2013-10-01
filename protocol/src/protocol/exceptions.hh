#ifndef INFINIT_PROTOCOL_CHECKSUMERROR_HH
# define INFINIT_PROTOCOL_CHECKSUMERROR_HH

# include <elle/Exception.hh>

namespace infinit
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
  }
}

#endif
