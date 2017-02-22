#ifndef INFINIT_REACTOR_EXCEPTION_HH
# define INFINIT_REACTOR_EXCEPTION_HH

# include <elle/Exception.hh>
# include <elle/Error.hh>

# include <reactor/duration.hh>
# include <reactor/fwd.hh>

namespace reactor
{
  class Exception: public elle::Exception
  {
  public:
    Exception(const std::string& message);
    Exception(elle::Backtrace const& bt, elle::String const& format);
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

#endif
