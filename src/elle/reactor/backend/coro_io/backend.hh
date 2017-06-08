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
          public backend::Backend
        {
        /*------.
        | Types |
        `------*/
        public:
          using Self = Backend;
          using Super = backend::Backend;
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
          std::unique_ptr<backend::Thread>
          make_thread(const std::string& name, Action action) override;
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
