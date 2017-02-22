#pragma once

#include <string>
#include <elle/reactor/backend/backend.hh>

struct Coro;

namespace elle
{
  namespace reactor
  {
    namespace backend
    {
      namespace coro_io
      {
        class Backend:
          public reactor::backend::Backend
        {
        /*------.
        | Types |
        `------*/
        public:
          typedef Backend Self;
          typedef reactor::backend::Backend Super;
          class Thread;

        /*-------------.
        | Construction |
        `-------------*/
        public:
          Backend();
          ~Backend();

        /*--------.
        | Threads |
        `--------*/
        public:
          virtual
          std::unique_ptr<backend::Thread>
          make_thread(const std::string& name, Action action) override;
          virtual
          backend::Thread*
          current() const override;

        /*--------.
        | Details |
        `--------*/
        private:
          /// Let threads manipulate the current thread and the root thread.
          friend class Thread;
          /// Root thread, which instantiated the Backend.
          std::unique_ptr<Thread> _self;
          /// Current thread.
          Thread* _current;
        };
      }
    }
  }
}
