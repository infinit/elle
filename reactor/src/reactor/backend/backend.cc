#include <reactor/backend/backend.hh>

#if defined(__arm__) || defined(__clang__)
// libc++

#include <reactor/libcxx-exceptions/cxa_exception.hpp>
#define THROW_SPEC

#else
//libstdc++
#include <reactor/libcxx-exceptions/unwind-cxx.h>
#define THROW_SPEC throw()

#endif


namespace reactor
{
  namespace backend
  {
    /*--------.
    | Backend |
    `--------*/

    Backend::~Backend()
    {}

    /*-------------.
    | Construction |
    `-------------*/

    Thread::Thread(const std::string& name,
                   Action action):
      _action(std::move(action)),
      _name(name),
      _status(Status::starting),
      _unwinding(false),
      _exception_storage(new __cxxabiv1::__cxa_eh_globals())
    {}

    Thread::~Thread()
    {
      delete (__cxxabiv1::__cxa_eh_globals*) _exception_storage;
    }

    /*------.
    | State |
    `------*/

    void
    Thread::status(Status status)
    {
      this->_status = status;
    }
  }
}
