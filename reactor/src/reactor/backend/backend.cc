#include <reactor/backend/backend.hh>

#if defined(__arm__) || defined(INFINIT_MACOSX) || defined(INFINIT_IOS)

#include <reactor/libcxx-exceptions/cxa_exception.hpp>

#elif defined(INFINIT_LINUX) || defined(INFINIT_WINDOWS)
#include <reactor/libcxx-exceptions/unwind-cxx.h>
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
                   const Action& action):
      _action(action),
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
