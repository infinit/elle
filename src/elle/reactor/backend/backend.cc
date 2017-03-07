#include <elle/reactor/backend/backend.hh>
#if defined(INFINIT_MACOSX)
// libc++
# include <elle/reactor/libcxx-exceptions/cxa_exception.hpp>
#include <utility>
# define THROW_SPEC
#else
//libstdc++
# include <elle/reactor/libcxx-exceptions/unwind-cxx.h>
# define THROW_SPEC throw()
#endif

namespace elle
{
  namespace reactor
  {
    namespace backend
    {
      /*--------.
      | Backend |
      `--------*/

      Backend::~Backend()
      = default;

      /*-------------.
      | Construction |
      `-------------*/

      Thread::Thread(std::string  name,
                     Action action):
        _action(std::move(action)),
        _name(std::move(name)),
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
}
