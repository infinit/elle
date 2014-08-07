#include <boost/coroutine/all.hpp>

#include <elle/Backtrace.hh>
#include <elle/assert.hh>
#include <elle/log.hh>

#include <reactor/backend/coro_io/backend.hh>
#include <reactor/exception.hh>

ELLE_LOG_COMPONENT("reactor.backend");

namespace reactor
{
  namespace backend
  {
    namespace coro_io
    {
      /*-------.
      | Thread |
      `-------*/

      class Backend::Thread:
        public backend::Thread
      {
      /*---------.
      | Typedefs |
      `---------*/
      public:
        typedef Thread Self;
        typedef backend::Thread Super;

      /*-------------.
      | Construction |
      `-------------*/
      public:
        Thread(Backend& backend,
               const std::string& name,
               const Action& action):
          Super(name, action),
          _backend(backend),
          _coro(new Coro(
            [&]
            (Sink& sink)
            {
              this->_sink = &sink;
              sink();
              this->_run();
            })),
          _caller(nullptr),
          _root(false),
          _unwinding(false)
        {}

        ~Thread()
        {
          ELLE_ASSERT(this->status() == Status::done ||
                      this->status() == Status::starting ||
                      this->_root);
          ELLE_TRACE("%s: die", *this);
          if (_coro)
          {
            delete _coro;
            _coro = nullptr;
            _sink = nullptr;
          }
        }

      private:
        Thread(Backend& backend)
          : Super("<root>", Action())
          , _backend(backend)
          , _coro(new Coro(
              [&]
              (Sink& sink)
              {
                while (true)
                {
                  sink();
                }
              }))
          , _caller(nullptr)
          , _root(true)
          , _unwinding(false)
        {
          this->status(Status::running);
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
          _caller = current;
          _backend._current = this;
          current->_unwinding = std::uncaught_exception();
          if (current->_unwinding)
            ELLE_DUMP("step %s with in-flight exception", *current);
          if (starting)
          {
            this->status(Status::running);
            ELLE_ASSERT_NEQ(_coro, nullptr);
            ELLE_ASSERT_NEQ(_sink, nullptr);
            ELLE_TRACE("%s: start %s", *current , *this);
            (*_coro)();
            ELLE_TRACE("%s: back from %s", *current, *this);
          }
          else
          {
            ELLE_ASSERT_EQ(this->status(), Status::waiting);
            this->status(Status::running);
            ELLE_TRACE("%s: step from %s", *this, *_caller);
            if (this->_exception)
            {
              /* The stack we are switching to expects std::current_exception()
              * to return this->_exception (which contains current_exception()
              * at the time of yield).
              * So wrap the switch into a catch clause to trick the compiler
              * into restoring it.
              */
              try
              {
                std::rethrow_exception(this->_exception);
              }
              catch(...)
              {
                (*_coro)();
              }
            }
            else
              (*_coro)();
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
          _backend._current = _caller;
          ELLE_TRACE("%s: yield back to %s", *this, *_backend._current);
          _caller = nullptr;
          // Store current exception and stack unwinding state
          this->_unwinding = std::uncaught_exception();
          this->_exception = std::current_exception();
          if (this->_unwinding)
            ELLE_DUMP("yielding %s with in-flight exception", *this);
          (*_sink)();
          if (_backend._current->_unwinding != std::uncaught_exception())
            ELLE_TRACE("yield %s: unwind mismatch, expect %s, got %s",
              *_backend._current, _backend._current->_unwinding,
              std::uncaught_exception());
        }


      /*--------.
      | Details |
      `--------*/
      private:
        /// Define type of coroutine used.
        typedef boost::coroutines::coroutine<void>::pull_type Coro;
        typedef boost::coroutines::coroutine<void>::push_type Sink;
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
        }

        /// Owning backend.
        Backend& _backend;
        /// Underlying IO coroutine.
        Coro* _coro;
        /// Yield sink.
        Sink* _sink;
        /// The thread that stepped us.
        Thread* _caller;
        ELLE_ATTRIBUTE(bool, root);
        ELLE_ATTRIBUTE(bool, unwinding);
        ELLE_ATTRIBUTE(std::exception_ptr, exception); // stored when yielding
      };

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
      Backend::make_thread(const std::string& name,
                           const Action& action)
      {
        return std::unique_ptr<backend::Thread>(
          new Thread(*this, name, action));
      }

      Thread*
      Backend::current() const
      {
        return _current;
      }
    }
  }
}
