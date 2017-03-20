#pragma once

# include <elle/Error.hh>
# include <elle/Exception.hh>
# include <elle/reactor/duration.hh>
# include <elle/reactor/fwd.hh>

namespace elle
{
  namespace reactor
  {
    class Exception
      : public elle::Exception
    {
    public:
      Exception(const std::string& message);
      Exception(elle::Backtrace const& bt, std::string const& format);
    };

    class Timeout
      : public elle::Error
    {
    public:
      using Super = elle::Error;
      Timeout(reactor::Duration const& delay);
      ELLE_ATTRIBUTE_R(reactor::Duration, delay);
    };

    class Terminate
      : public elle::Exception
    {
    public:
      using Super = elle::Exception;
      Terminate(std::string const& message);
    };
  }
}
