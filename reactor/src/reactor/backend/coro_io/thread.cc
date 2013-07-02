#include <elle/Backtrace.hh>
#include <elle/assert.hh>
#include <elle/log.hh>

#include <reactor/backend/thread.hh>
#include <reactor/backend/coro_io/libcoroutine/coroutine.hh>
#include <reactor/backend/coro_io/thread.hh>
#include <reactor/exception.hh>

ELLE_LOG_COMPONENT("reactor.backend");

namespace reactor
{
  namespace backend
  {
    namespace coro_io
    {
      /*----------------------.
      | Manager: Construction |
      `----------------------*/

      Manager::Manager()
        : _self(*this)
        , _current(&_self)
      {}

      Manager::~Manager()
      {}

      /*---------------.
      | Manager: State |
      `---------------*/

      Thread*
      Manager::current() const
      {
        return _current;
      }

      /*----------------.
      | Thread: Helpers |
      `----------------*/

      void starter(void* arg)
      {
        Thread* thread = reinterpret_cast<Thread*>(arg);
        thread->_run();
      }

      /*---------------------.
      | Thread: Construction |
      `---------------------*/

      Thread::Thread(Manager& manager,
                     const std::string& name,
                     const Action& action)
        : _name(name)
        , _status(status::starting)
        , _manager(manager)
        , _action(action)
        , _coro(Coro_new())
        , _caller(0)
      {}

      Thread::Thread(Manager& manager)
        : Thread(manager, "<root>", Action())
      {
        _status = status::running;
        ELLE_ASSERT(_coro);
        Coro_initializeMainCoro(_coro);
      }

      Thread::~Thread()
      {
        ELLE_ASSERT(status() == status::done ||
                    status() == status::starting ||
                    this == &_manager._self);
        ELLE_TRACE("%s: die", this->_name);
        if (_coro)
          {
            Coro_free(_coro);
            _coro = 0;
          }
      }

      /*---------------.
      | Thread: Naming |
      `---------------*/

      std::string
      Thread::name() const
      {
        return _name;
      }

      Status
      Thread::status() const
      {
        return _status;
      }

      bool
      Thread::done() const
      {
        return _status == status::done;
      }

      /*------------------.
      | Thread: Switching |
      `------------------*/

      void
      Thread::step()
      {
        ELLE_ASSERT(_caller);
        if (this->_status == status::starting)
        {
          _status = status::running;
          Thread* current = _manager._current;
          _caller = current;
          _manager._current = this;
          ELLE_ASSERT(_coro);
          ELLE_TRACE("%s: start %s", current->_name , this->_name);
          Coro_startCoro_(_caller->_coro, _coro, this, &starter);
          ELLE_TRACE("%s: back from %s", current->_name, _name);
        }
        else
        {
          ELLE_ASSERT_EQ(_status, status::waiting);
          _status = status::running;
          Thread* current = _manager._current;
          _caller = current;
          _manager._current = this;
          ELLE_TRACE("%s: step from %s", this->_name, _caller->_name);
          Coro_switchTo_(current->_coro, _coro);
        }
      }

      /*------------------.
      | Thread: Details.  |
      `------------------*/

      void
      Thread::_run()
      {
        _status = status::running;
        try
        {
          ELLE_ASSERT(_action);
          _action();
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
        _caller = 0;
        _status = status::done;
        _manager._current = caller;
        ELLE_TRACE("%s: done", this->_name);
        Coro_switchTo_(_coro, caller->_coro);
      }

      void
      Thread::yield()
      {
        ELLE_ASSERT_EQ(_manager._current, this);
        ELLE_ASSERT_EQ(_status, status::running);
        _status = status::waiting;
        _manager._current = _caller;
        ELLE_TRACE("%s: yield back to %s",
                       this->_name, _manager._current->_name);
        _caller = 0;
        Coro_switchTo_(_coro, _manager._current->_coro);
      }
    }
  }
}
