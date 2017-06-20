#pragma once

#include <string>

#include <elle/reactor/backend/backend.hh>

namespace elle
{
  namespace reactor
  {
    namespace backend
    {
      namespace boost
      {
        class Backend
          : public reactor::backend::Backend
        {
        /*------.
        | Types |
        `------*/
        public:
          using Self = Backend;
          using Super = reactor::backend::Backend;
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
          make_thread(const std::string& name,
                      Action action) override;
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
