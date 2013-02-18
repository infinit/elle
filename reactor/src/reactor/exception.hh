#ifndef INFINIT_REACTOR_EXCEPTION_HH
# define INFINIT_REACTOR_EXCEPTION_HH

# include <elle/Exception.hh>

# include <reactor/fwd.hh>

#define INFINIT_REACTOR_EXCEPTION(Name)         \
  virtual void raise_and_delete() const         \
  {                                             \
    Name actual(*this);                         \
    delete this;                                \
    throw actual;                               \
  }

namespace reactor
{
  class Exception: public elle::Exception
  {
  public:
    Exception(const std::string& message);
    Exception(elle::Backtrace const& bt, elle::String const& format);
    virtual ~Exception() throw ();
    INFINIT_REACTOR_EXCEPTION(Exception);
    void raise();
  };

  class Terminate: public Exception
  {
  public:
    typedef Exception Super;
    Terminate();
    INFINIT_REACTOR_EXCEPTION(Terminate);
  };
}

#endif
