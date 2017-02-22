#include <boost/function.hpp>

#include <elle/assert.hh>
#include <elle/log.hh>
#include <elle/reactor/backend/pthread/pthread.hh>
#include <elle/reactor/backend/thread.hh>

ELLE_LOG_COMPONENT("elle.reactor.backend.Threads");

namespace elle
{
  namespace reactor
  {
    namespace backend
    {
      namespace pthread
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

        void*
        starter(void* arg)
        {
          Thread* thread = reinterpret_cast<Thread*>(arg);
          thread->_run();
          return 0;
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
          , _thread()
          , _cond()
          , _mutex()
          , _caller(0)
        {
          ELLE_TRACE("%s: spawn", this->_name);
          std::unique_lock<std::mutex> lock(_manager._mutex);
          _thread = boost::thread(boost::bind(&Thread::_run, this));
          _manager._cond.wait(lock);
        }

        Thread::Thread(Manager& manager)
          : _name("<root>")
          , _status(status::running)
          , _manager(manager)
          , _action()
          , _thread()
          , _cond()
          , _mutex()
          , _caller(0)
        {
          ELLE_TRACE("%s: spawn", this->_name);
        }

        Thread::~Thread()
        {
          ELLE_TRACE("%s: die", this->_name);
          ELLE_ASSERT(_status == status::done || this == &_manager._self);
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
          ELLE_ASSERT(_status == status::waiting || _status == status::starting);
          ELLE_ASSERT(_caller == 0);
          _status = status::running;
          Thread* current = _manager._current;
          _caller = current;
          _manager._current = this;
          std::unique_lock<std::mutex> lock(current->_mutex);
          ELLE_TRACE("%s: step from %s", this->_name, _caller->_name);
          {
            std::unique_lock<std::mutex> lock(_mutex);
            _cond.notify_one();
          }
          current->_cond.wait(lock);
          if (_status == status::done)
            _thread.join();
        }

        /*------------------.
        | Thread: Details.  |
        `------------------*/

        void
        Thread::_run()
        {
          {
            std::unique_lock<std::mutex> lock(_mutex);
            {
              std::unique_lock<std::mutex> lock(_manager._mutex);
              _manager._cond.notify_one();
            }
            ELLE_TRACE("%s: ready to go", this->_name);
            _cond.wait(lock);
          }
          _status = status::running;
          try
          {
            _action();
          }
          catch (const std::exception& e)
          {
            std::cerr << "Thread " << name()
                      << " killed by exception: " << e.what()
                      << "." << std::endl;
            std::abort();
          }
          catch (...)
          {
            std::cerr << "Thread " << name()
                      << " killed by unknown exception." << std::endl;
            std::abort();
          }
          Thread* caller = _caller;
          _caller = 0;
          _status = status::done;
          ELLE_TRACE("%s: done", this->_name);
          _manager._current = caller;
          {
            std::unique_lock<std::mutex> lock(caller->_mutex);
            caller->_cond.notify_one();
          }
        }

        void
        Thread::yield()
        {
          ELLE_ASSERT_EQ(_manager._current, this);
          ELLE_ASSERT_EQ(_status, status::running);
          std::unique_lock<std::mutex> lock(_mutex);
          _status = status::waiting;
          _manager._current = _caller;
          ELLE_TRACE("%s: yield back to %s",
                         this->_name, _manager._current->_name);
          _caller = 0;
          {
            std::unique_lock<std::mutex> lock(_manager._current->_mutex);
            _manager._current->_cond.notify_one();
          }
          _cond.wait(lock);
        }
      }
    }
  }
}
