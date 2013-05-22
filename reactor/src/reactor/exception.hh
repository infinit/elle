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
  };

  class Terminate: public std::exception
  {
  private:
    std::string _message;
  public:
    typedef std::exception Super;
    Terminate(const std::string& message);
    virtual char const* what() const noexcept;
  };
}

#endif
