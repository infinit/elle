#include <boost/foreach.hpp>

#include <elle/finally.hh>
#include <elle/log.hh>
#include <elle/container/set.hh>

#include <reactor/exception.hh>
#include <reactor/scheduler.hh>
#include <reactor/signal.hh>
#include <reactor/sleep.hh>
#include <reactor/thread.hh>

ELLE_LOG_COMPONENT("reactor.Thread");

namespace reactor
{
  /*-------------.
  | Construction |
  `-------------*/

  Thread::Thread(Scheduler&             scheduler,
                 std::string const&     name,
                 Action const&          action,
                 bool                   dispose)
    : _dispose(dispose)
    , _state(state::running)
    , _injection()
    , _exception()
    , _backtrace_root()
    , _waited()
    , _timeout(false)
    , _timeout_timer(scheduler.io_service())
    , _thread(scheduler._manager, name, boost::bind(&Thread::_action_wrapper,
                                                    this, action))
    , _scheduler(scheduler)
  {
    ELLE_ASSERT(action);
    _scheduler._thread_register(*this);
  }

  Thread::~Thread()
  {
    ELLE_ASSERT_EQ(state(), state::done);
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
  Thread::print(std::ostream& s) const
  {
    s << "thread " << name();
  }

  void
  Thread::Dump(std::ostream& s) const
  {
    s << "Thread " << name()
      << " @" << this
      << " [ State = " << state() << "]"
      << " waited by " << this->_threads;
  }

  /*----.
  | Run |
  `----*/

  void
  Thread::_action_wrapper(const Thread::Action& action)
  {
    try
    {
      _backtrace_root = elle::Backtrace::current();
      ELLE_ASSERT(action);
      action();
    }
    catch (const Terminate&)
    {}
    catch (...)
    {
      ELLE_WARN("%s: exception escaped: %s", *this, elle::exception_string());
      _exception_thrown = std::current_exception();
    }
  }

  void
  Thread::_step()
  {
    _thread.step();
    if (_thread.status() == backend::status::done)
    {
      ELLE_TRACE_SCOPE("%s: done", *this);
      _state = Thread::state::done;
      _signal();
    }
    if (this->_exception_thrown)
    {
      ELLE_TRACE("%s: re-raise exception: %s",
                 *this, elle::exception_string(this->_exception_thrown));
      std::rethrow_exception(this->_exception_thrown);
    }
  }

  void
  Thread::yield()
  {
    ELLE_TRACE("%s: yield", *this)
    {
      _thread.yield();
      ELLE_TRACE_SCOPE("%s: back from yield", *this);
      if (_injection)
      {
        ELLE_TRACE("%s: inject code", *this);
        Injection i(_injection);
        _injection = Injection();
        i();
      }
      if (_exception)
      {
        ELLE_TRACE("%s: re-raise exception: %s",
                   *this, elle::exception_string(this->_exception));
        std::exception_ptr tmp = this->_exception;
        this->_exception = std::exception_ptr{};
        std::rethrow_exception(tmp);
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

  /*----------------.
  | Synchronization |
  `----------------*/

  bool
  Thread::wait(Waitable& s,
               DurationOpt timeout)
  {
    Waitables waitables(1, &s);
    return wait(waitables, timeout);
  }

  bool
  Thread::wait(Waitables const& waitables,
               DurationOpt timeout)
  {
    ELLE_TRACE_SCOPE("%s: wait %s%s", *this, waitables,
                     timeout ? elle::sprintf(" for %s", timeout) : "");
    ELLE_ASSERT_EQ(_state, state::running);
    ELLE_ASSERT(_waited.empty());
    bool freeze = false;
    BOOST_FOREACH (Waitable* s, waitables)
      if (s->_wait(this))
      {
        freeze = true;
        _waited.insert(s);
      }
    if (freeze)
    {
      if (timeout)
      {
        this->_timeout_timer.expires_from_now(timeout.get());
        this->_timeout = false;
        this->_timeout_timer.async_wait(
          boost::bind(&Thread::_wait_timeout, this, _1));
        auto cancel_timeout = [&]
          {
            if (!_timeout)
              this->_timeout_timer.cancel();
          };
        return elle::With<elle::Finally>(cancel_timeout) << [&]
        {
          _freeze();
          return !this->_timeout;
        };
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
    // If we're not frozen anymore, the task must have ended in the same asio
    // poll than the timeout: Thread::_wake was just called. Ignore the timeout.
    if (state() != state::frozen)
      return;
    ELLE_TRACE("%s: timed out", *this);
    this->_timeout = true;
    this->_wait_abort();
  }

  void
  Thread::_wait_abort()
  {
    ELLE_TRACE("%s: abort wait", *this);
    ELLE_ASSERT_EQ(state(), state::frozen);
    BOOST_FOREACH (Waitable* waitable, _waited)
      waitable->_unwait(this);
    this->_waited.clear();
    this->_timeout_timer.cancel();
    this->_scheduler._unfreeze(*this);
    this->_state = Thread::state::running;
  }

  void
  Thread::_freeze()
  {
    ELLE_TRACE_SCOPE("%s: freeze", *this);
    _scheduler._freeze(*this);
    _state = Thread::state::frozen;
    yield();
  }

  void
  Thread::_wake(Waitable*       waitable)
  {
    ELLE_TRACE("%s: wait ended for %s", *this, *waitable)
      {
        if (waitable->_exception && !_exception)
          {
            ELLE_TRACE("%s: forward exception", *this);
            _exception = waitable->_exception;
          }
        _waited.erase(waitable);
        if (_waited.empty())
          {
            ELLE_TRACE("%s: nothing to wait on, waking up", *this);
            _scheduler._unfreeze(*this);
            _state = Thread::state::running;
          }
        else
          ELLE_TRACE("%s: still waiting for %s other elements", *this, _waited.size());
      }
  }

  /*--------.
  | Backend |
  `--------*/

  Scheduler&
  Thread::scheduler()
  {
    return _scheduler;
  }

  /*----------------.
  | Print operators |
  `----------------*/

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
