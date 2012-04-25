#ifndef INFINIT_REACTOR_EXCEPTION_HH
# define INFINIT_REACTOR_EXCEPTION_HH

# include <stdexcept>

#define INFINIT_REACTOR_EXCEPTION(Name)         \
  virtual void raise_and_delete() const         \
  {                                             \
    Name actual(*this);                         \
    delete this;                                \
    throw actual;                               \
  }                                             \

namespace reactor
{
  class Exception: public std::runtime_error
  {
  public:
    Exception(const std::string& message);
    INFINIT_REACTOR_EXCEPTION(Exception);
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
