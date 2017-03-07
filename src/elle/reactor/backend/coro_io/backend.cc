#include <elle/Backtrace.hh>
#include <elle/assert.hh>
#include <elle/log.hh>
#include <elle/reactor/backend/coro_io/backend.hh>
#include <elle/reactor/backend/coro_io/libcoroutine/coroutine.hh>
#include <elle/reactor/exception.hh>

ELLE_LOG_COMPONENT("elle.reactor.backend.coro_io");

namespace elle
{
  namespace reactor
  {
    namespace backend
    {
      namespace coro_io
      {
        /*-------.
        | Thread |
        `-------*/

        static
        void
        starter(void* arg);

        class Backend::Thread:
          public backend::Thread
        {
        /*---------.
        | Typedefs |
        `---------*/
        public:
          using Self = Thread;
          using Super = backend::Thread;

        /*-------------.
        | Construction |
        `-------------*/
        public:
          Thread(Backend& backend,
                 const std::string& name,
                 Action action):
            Super(name, std::move(action)),
            _backend(backend),
            _coro(Coro_new()),
            _caller(nullptr),
            _root(false)
          {}

          ~Thread()
          {
            ELLE_ASSERT(this->status() == Status::done ||
                        this->status() == Status::starting ||
                        this->_root);
            ELLE_TRACE("%s: die", *this);
            if (_coro)
            {
              Coro_free(_coro);
              _coro = nullptr;
            }
          }

        private:
          Thread(Backend& backend):
            Thread(backend, "<root>", Action())
          {
            this->_root = true;
            this->status(Status::running);
            ELLE_ASSERT(_coro);
            Coro_initializeMainCoro(_coro);
          }

        /*----------.
        | Switching |
        `----------*/
        public:
          virtual
          void
          step() override
          {
            // go from current to this
            ELLE_ASSERT(_caller == nullptr);

            bool starting = this->status() == Status::starting;
            Thread* current = _backend._current;
            current->_unwinding = std::uncaught_exception();
            _caller = current;
            _backend._current = this;
            if (current->_unwinding)
              ELLE_DUMP("step %s with in-flight exception", *current);
            if (starting)
            {
              this->status(Status::running);
              ELLE_ASSERT(_coro);
              ELLE_TRACE("%s: start %s", *current , *this);
              Coro_startCoro_(_caller->_coro, _coro, this, &starter);
              ELLE_TRACE("%s: back from %s", *current, *this);
            }
            else
            {
              ELLE_ASSERT_EQ(this->status(), Status::waiting);
              this->status(Status::running);
              ELLE_TRACE("%s: step from %s", *this, *_caller);
              Coro_switchTo_(current->_coro, _coro);
            }
            // It is unclear whether an uncaught_exception mismatch has any
            // consequence if the code does not explicitly depend on its result.
            // Warn about it just in case.
            if (_backend._current->_unwinding != std::uncaught_exception())
              ELLE_TRACE("step %s: unwind mismatch, expect %s, got %s",
                *_backend._current, _backend._current->_unwinding,
                std::uncaught_exception());
          }

          virtual
          void
          yield() override
          {
            // go from this to caller
            ELLE_ASSERT_EQ(_backend._current, this);
            ELLE_ASSERT_EQ(this->status(), Status::running);
            this->status(Status::waiting);
            // Store current exception and stack unwinding state
            this->_unwinding = std::uncaught_exception();
            this->_exception = std::current_exception();
            _backend._current = _caller;
            ELLE_TRACE("%s: yield back to %s", *this, *_backend._current);
            _caller = nullptr;

            if (this->_unwinding)
              ELLE_DUMP("yielding %s with in-flight exception", *this);
            Coro_switchTo_(_coro, _backend._current->_coro);
            if (_backend._current->_unwinding != std::uncaught_exception())
              ELLE_TRACE("yield %s: unwind mismatch, expect %s, got %s",
                *_backend._current, _backend._current->_unwinding,
                std::uncaught_exception());
          }


        /*--------.
        | Details |
        `--------*/
        private:
          /// Let the backend use our private constructor.
          friend class Backend;
          /// Main routine of this thread of execution.
          void
          _run()
          {
            this->status(Status::running);
            try
            {
              this->action()();
            }
            catch (reactor::Exception const& e)
            {
              std::cerr << "Thread " << name()
                        << " killed by exception "
                        << elle::demangle(typeid(e).name()) << ": "
                        << e.what() << "." << std::endl;
              std::cerr << e.backtrace() << std::endl;
              std::abort();
            }
            catch (const std::exception& e)
            {
              std::cerr << "Thread " << name()
                        << " killed by exception "
                        << elle::demangle(typeid(e).name()) << ": "
                        << e.what() << "." << std::endl;
              std::abort();
            }
            catch (...)
            {
              std::cerr << "Thread " << name() << " killed by unknown exception."
                        << std::endl;
              std::abort();
            }
            Thread* caller = _caller;
            _caller = nullptr;
            this->status(Status::done);
            _backend._current = caller;
            ELLE_TRACE("%s: done", *this);
            Coro_switchTo_(_coro, caller->_coro);
          }

          /// Owning backend.
          Backend& _backend;
          /// Underlying IO coroutine.
          struct Coro* _coro;
          /// The thread that stepped us.
          Thread* _caller;
          /// Let libcoroutine callback invoke our _run.
          friend void starter(void* arg);
          ELLE_ATTRIBUTE(bool, root);
        };

        static
        void
        starter(void* arg)
        {
          Backend::Thread* thread = reinterpret_cast<Backend::Thread*>(arg);
          thread->_run();
        }

        /*--------.
        | Backend |
        `--------*/

        Backend::Backend():
          _self(new Thread(*this)),
          _current(_self.get())
        {}

        Backend::~Backend()
        {}

        std::unique_ptr<backend::Thread>
        Backend::make_thread(const std::string& name, Action action)
        {
          return std::unique_ptr<backend::Thread>(
            new Thread(*this, name, std::move(action)));
        }

        Thread*
        Backend::current() const
        {
          return _current;
        }
      }
    }
  }
}
