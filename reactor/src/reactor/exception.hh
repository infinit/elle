#ifndef INFINIT_REACTOR_EXCEPTION_HH
# define INFINIT_REACTOR_EXCEPTION_HH

# include <elle/Exception.hh>

# include <reactor/fwd.hh>

namespace reactor
{
  class Exception: public elle::Exception
  {
  public:
    Exception(const std::string& message);
    Exception(elle::Backtrace const& bt, elle::String const& format);
    ELLE_EXCEPTION(Exception);
  };

  class Terminate: public Exception
  {
  public:
    typedef Exception Super;
    Terminate(const std::string& message);
    ELLE_EXCEPTION(Terminate);
  };
}

#endif
