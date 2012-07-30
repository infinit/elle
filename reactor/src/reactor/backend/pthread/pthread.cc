#include <boost/format.hpp>
#include <boost/function.hpp>

#include <reactor/backend/thread.hh>
#include <reactor/backend/pthread/pthread.hh>

#include <elle/log.hh>

ELLE_LOG_TRACE_COMPONENT("Reactor.Backend.Threads");

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
        ELLE_LOG_TRACE("%s: spawn", this->_name);
        boost::unique_lock<boost::mutex> lock(_manager._mutex);
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
        ELLE_LOG_TRACE("%s: spawn", this->_name);
      }

      Thread::~Thread()
      {
        ELLE_LOG_TRACE("%s: die", this->_name);
        assert(_status == status::done || this == &_manager._self);
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
        assert(_status == status::waiting || _status == status::starting);
        assert(_caller == 0);
        _status = status::running;
        Thread* current = _manager._current;
        _caller = current;
        _manager._current = this;
        boost::unique_lock<boost::mutex> lock(current->_mutex);
        ELLE_LOG_TRACE("%s: step from %s", this->_name, _caller->_name);
        {
          boost::unique_lock<boost::mutex> lock(_mutex);
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
          boost::unique_lock<boost::mutex> lock(_mutex);
          {
            boost::unique_lock<boost::mutex> lock(_manager._mutex);
            _manager._cond.notify_one();
          }
          ELLE_LOG_TRACE("%s: ready to go", this->_name);
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
        ELLE_LOG_TRACE("%s: done", this->_name);
        _manager._current = caller;
        {
          boost::unique_lock<boost::mutex> lock(caller->_mutex);
          caller->_cond.notify_one();
        }
      }

      void
      Thread::yield()
      {
        assert(_manager._current == this);
        assert(_status == status::running);
        boost::unique_lock<boost::mutex> lock(_mutex);
        _status = status::waiting;
        _manager._current = _caller;
        ELLE_LOG_TRACE("%s: yield back to %s",
                       this->_name, _manager._current->_name);
        _caller = 0;
        {
          boost::unique_lock<boost::mutex> lock(_manager._current->_mutex);
          _manager._current->_cond.notify_one();
        }
        _cond.wait(lock);
      }
    }
  }
}
