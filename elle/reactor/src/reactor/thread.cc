#include <boost/foreach.hpp>

#include <elle/log.hh>

#include <reactor/debug.hh>
#include <reactor/scheduler.hh>
#include <reactor/signal.hh>
#include <reactor/sleep.hh>
#include <reactor/thread.hh>

ELLE_LOG_TRACE_COMPONENT("Reactor.Thread");

namespace reactor
{
  /*-------------.
  | Construction |
  `-------------*/

  static void action_wrapper(Thread::Action action);

  Thread::Thread(Scheduler&             scheduler,
                 std::string const&     name,
                 Action const&          action,
                 bool                   dispose)
    : _dispose(dispose)
    , _state(state::running)
    , _injection()
    , _exception(0)
    , _waited()
    , _timeout(false)
    , _thread(scheduler._manager, name, boost::bind(action_wrapper,
                                                    action))
    , _scheduler(scheduler)
  {
    assert(action);
    _scheduler._thread_register(*this);
  }

  Thread::~Thread()
  {
    assert(state() == state::done);
    _destructed();
  }

  /*---------.
  | Tracking |
  `---------*/

  Thread::Tracker&
  Thread::destructed()
  {
    return _destructed;
  }

  /*-------.
  | Status |
  `-------*/

  Thread::State
  Thread::state() const
  {
    return _state;
  }

  bool
  Thread::done() const
  {
    return state() == state::done;
  }

  std::string
  Thread::name() const
  {
    return _thread.name();
  }

  void
  Thread::Print(std::ostream& s) const
  {
    s << "thread " << name();
  }

  void
  Thread::Dump(std::ostream& s) const
  {
    s << "Thread " << name() << " [ State = " << state() << "]";
  }

  /*----.
  | Run |
  `----*/


  static void action_wrapper(Thread::Action action)
  {
    try
      {
        assert(action);
        action();
      }
    catch (const Terminate&)
      {}
  }

  void
  Thread::_step()
  {
    _thread.step();
    if (_thread.status() == backend::status::done)
    {
      _state = Thread::state::done;
      _signal();
    }
  }

  void
  Thread::yield()
  {
    ELLE_LOG_TRACE("%s: yield", *this)
      {
        _thread.yield();
        ELLE_LOG_TRACE("%s: back from yield", *this);
        if (_injection)
          {
            Injection i(_injection);
            _injection = Injection();
            i();
          }
        if (_exception)
          {
            Exception* e = _exception;
            _exception = 0;
            ELLE_LOG_TRACE("%s: re-raise exception", *this);
            e->raise_and_delete();
          }
      }
  }

  /*-----------.
  | Injections |
  `-----------*/

  void
  Thread::inject(Injection const& injection)
  {
    _injection = injection;
  }

  void
  Thread::sleep(Duration d)
  {
    Sleep sleep(_scheduler, d);
    sleep.run();
  }

  void
  Thread::raise(Exception* e)
  {
    _exception = e;
  }

  /*----------------.
  | Synchronization |
  `----------------*/

  bool
  Thread::wait(Waitable&                        s,
               boost::optional<Duration>        timeout)
  {
    Waitables waitables(1, &s);
    return wait(waitables, timeout);
  }

  bool
  Thread::wait(Waitables&                       waitables,
               boost::optional<Duration>        timeout)
  {
    assert(_state == state::running);
    assert(_waited.empty());
    bool freeze = false;
    BOOST_FOREACH (Waitable* s, waitables)
      if (s->_wait(this))
      {
        freeze = true;
        _waited.insert(s);
      }
    if (freeze)
    {
      ELLE_LOG_TRACE("%s: freeze", *this);
      if (timeout)
      {
        boost::asio::deadline_timer _timer(_scheduler.io_service(),
                                           timeout.get());
        _timeout = false;
        _timer.async_wait(boost::bind(&Thread::_wait_timeout, this, _1));
        try
          {
            _freeze();
          }
        catch (const Terminate&)
          {
            _timer.cancel();
            throw;
          }
        if (_timeout)
          return false;
        else
        {
          _timer.cancel();
          return true;
        }
      }
      else
      {
        _freeze();
        return true;
      }
    }
    else
      return true;
  }

  void Thread::terminate()
  {
    _scheduler._terminate(this);
  }

  void Thread::terminate_now()
  {
    _scheduler._terminate_now(this);
  }

  bool
  Thread::_wait(Thread* thread)
  {
    if (_state == state::done)
      return false;
    else
      return Waitable::_wait(thread);
  }

  void
  Thread::_wait_timeout(boost::system::error_code const& e)
  {
    if (e == boost::system::errc::operation_canceled)
      return;
    ELLE_LOG_TRACE("%s: timed out", *this);
    _wait_abort();
  }

  void
  Thread::_wait_abort()
  {
    ELLE_LOG_TRACE("%s: abort wait", *this);
    assert(state() == state::frozen);
    BOOST_FOREACH (Waitable* waitable, _waited)
      waitable->_unwait(this);
    _waited.clear();
    _timeout = true;
    _scheduler._unfreeze(*this);
    _state = Thread::state::running;
  }

  void
  Thread::_freeze()
  {
    _scheduler._freeze(*this);
    _state = Thread::state::frozen;
    yield();
  }

  void
  Thread::_wake(Waitable*       waitable)
  {
    ELLE_LOG_TRACE("%s: wait ended for %s", *this, *waitable)
      {
        if (waitable->_exception && !_exception)
          {
            ELLE_LOG_TRACE("%s: forward exception", *this);
            _exception = waitable->_exception;
          }
        _waited.erase(waitable);
        if (_waited.empty())
          {
            ELLE_LOG_TRACE("%s: nothing to wait on, waking up", *this);
            _scheduler._unfreeze(*this);
            _state = Thread::state::running;
          }
        else
          ELLE_LOG_TRACE("%s: still waiting for %s other elements", *this, _waited.size());
      }
  }

  /*----------------.
  | Print operators |
  `----------------*/

  std::ostream&
  operator <<(std::ostream&     s,
              Thread const&     t)
  {
    t.Print(s);
    return s;
  }

  std::ostream&
  operator <<(std::ostream&     s,
              Thread::State     state)
  {
    switch (state)
    {
      case Thread::state::done:
        s << "done";
        break;
      case Thread::state::frozen:
        s << "frozen";
        break;
      case Thread::state::running:
        s << "running";
        break;
    }
    return s;
  }
}
