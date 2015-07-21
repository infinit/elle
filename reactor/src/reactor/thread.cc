#include <reactor/thread.hh>

#include <boost/foreach.hpp>

#include <elle/finally.hh>
#include <elle/log.hh>
#include <elle/container/set.hh>

#include <reactor/backend/backend.hh>
#include <reactor/exception.hh>
#include <reactor/scheduler.hh>
#include <reactor/signal.hh>
#include <reactor/sleep.hh>

ELLE_LOG_COMPONENT("reactor.Thread");

// FIXME: bug hunting, remove
#include <reactor/http/Request.hh>

namespace reactor
{
  /*-------------.
  | Construction |
  `-------------*/

  Thread::Thread(Scheduler& scheduler,
                 std::string const& name,
                 Action const& action,
                 bool dispose)
    : _dispose(dispose)
    , _state(state::running)
    , _injection()
    , _exception()
    , _backtrace_root()
    , _waited()
    , _timeout(false)
    , _timeout_timer(scheduler.io_service())
    , _thread(scheduler._manager->make_thread(
                name,
                boost::bind(&Thread::_action_wrapper, this, action)))
    , _scheduler(scheduler)
    , _terminating(false)
  {
    _scheduler._thread_register(*this);
  }

  Thread::Thread(std::string const& name,
                 Action const& action,
                 bool dispose):
    Thread(reactor::scheduler(), name, action, dispose)
  {}

  ThreadPtr
  Thread::make_tracked(const std::string& name,
                       const Action& action)
  {
    return make_tracked(*reactor::Scheduler::scheduler(), name, action);
  }

  ThreadPtr
  Thread::make_tracked(Scheduler& scheduler,
                       const std::string& name,
                       const Action& action)
  {
    ThreadPtr res = std::make_shared<Thread>(scheduler, name, action);
    res->_self = res;
    return res;
  }

  Thread::~Thread()
  {
    if (this->state() != state::done)
      ELLE_ABORT("%s: destroyed in state %s", *this, this->state());
    _destructed();
  }

  void
  Thread::_scheduler_release()
  {
    ELLE_DUMP("%s: scheduler_release, dispose=%s", *this, this->_dispose);
    this->_released();
    this->_released.disconnect_all_slots();
    this->_released();
    if (this->_dispose)
      delete this;
    else /* the else is not an option*/
      this->_self.reset();
  }

  /*---------.
  | Tracking |
  `---------*/

  Thread::Tracker&
  Thread::destructed()
  {
    return _destructed;
  }

  Thread::Tracker&
  Thread::released()
  {
    return _released;
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
    return _thread->name();
  }

  void
  Thread::print(std::ostream& s) const
  {
    s << "thread " << name();
  }

  static
  std::ostream&
  operator <<(std::ostream& s, const Waitable::Waiters& w)
  {
    s << '[';
    for (auto const& t: w.get<0>())
      s << t << ", ";
    s << ']';
    return s;
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
      if (this->_exception)
      {
        ELLE_TRACE("%s: wrapper: re-raise exception: %s",
                   *this, elle::exception_string(this->_exception));
        std::exception_ptr tmp = this->_exception;
        this->_exception = std::exception_ptr{};
        std::rethrow_exception(tmp);
      }
      this->_backtrace_root = elle::Backtrace::current();
      action();
    }
    catch (Terminate const&)
    {}
    catch (elle::Exception const& e)
    {
      ELLE_TRACE_SCOPE("%s: exception escaped: %s",
                       *this, elle::exception_string());
      ELLE_DUMP("exception type: %s", elle::demangle(typeid(e).name()));
      ELLE_DUMP("backtrace:\n%s", e.backtrace());
      this->_exception_thrown = std::current_exception();
    }
    catch (...)
    {
      ELLE_WARN("%s: exception escaped: %s", *this, elle::exception_string());
      this->_exception_thrown = std::current_exception();
    }
  }

  void
  Thread::_step()
  {
    this->_thread->step();
    if (this->_thread->status() == backend::Thread::Status::done)
    {
      ELLE_TRACE_SCOPE("%s: done", *this);
      _state = Thread::state::done;
      _signal();
    }
    // Unfortunately, uncaught_exception is broken by elle::exception_string,
    // probably because of the coroutines. True on GCC libstdc++ 4.8 at least.
    else if (this->_terminating
             && !this->_thread->unwinding()
             && !this->_thread->exception())
    {
      ELLE_ERR("%s: terminate exception was swallowed", *this);
      this->raise<Terminate>(elle::sprintf("re-terminate %s", *this));
      this->_wait_abort("terminate exception was swallowed");
    }
    if (this->_exception_thrown)
    {
      ELLE_TRACE("%s: step: re-raise exception: %s",
                 *this, elle::exception_string(this->_exception_thrown));
      // Do not reraise in the context of this thread
      this->_scheduler._current = nullptr;
      std::rethrow_exception(this->_exception_thrown);
    }
  }

  void
  Thread::yield()
  {
    ELLE_TRACE("%s: yield", *this)
    {
      this->_thread->yield();
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
        ELLE_TRACE("%s: yield: re-raise exception: %s",
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
#ifndef INFINIT_IOS
    ELLE_TRACE_SCOPE("%s: wait %s%s", *this, waitables,
                     timeout ? elle::sprintf(" for %s", timeout) : "");
#endif
    ELLE_ASSERT_EQ(_state, state::running);
    ELLE_ASSERT(_waited.empty());
    bool freeze = false;
    BOOST_FOREACH (Waitable* s, waitables)
      if (s->_wait(this))
      {
        freeze = true;
        _waited.insert(s);
      }
      else if (s->_exception)
      {
        for (Waitable* waitable: this->_waited)
          waitable->_unwait(this);
        ELLE_ASSERT(this->_waited.empty());
        std::rethrow_exception(s->_exception);
      }

    if (freeze)
    {
      if (timeout)
      {
        this->_timeout_timer.expires_from_now(timeout.get());
        this->_timeout = false;
        auto repr = elle::sprintf("%s", waitables);
        this->_timeout_timer.async_wait(
          [this, repr]
          (boost::system::error_code const& e)
          {
            this->_wait_timeout(e, repr);
          });
        auto cancel_timeout = [this]
          {
            ELLE_DUMP("%s: cancel timeout", *this);
            if (!this->_timeout)
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

  void Thread::terminate_now(bool suicide)
  {
    _scheduler._terminate_now(this, suicide);
  }

  bool
  Thread::_wait(Thread* thread)
  {
    if (_state == state::done)
      return false;
    else
      return Waitable::_wait(thread);
  }

  static
  std::ostream&
  operator <<(std::ostream& output, std::set<Waitable*> waitables)
  {
    if (waitables.size() == 1)
      output << **waitables.begin();
    else
    {
      bool first = true;
      for (auto const& waitable: waitables)
      {
        if (first)
          first = false;
        else
          output << ", ";
        output << *waitable;
      }
      output << "}";
    }
    return output;
  }

  void
  Thread::_wait_timeout(boost::system::error_code const& e,
                        std::string const& waited)
  {
    if (e == boost::system::errc::operation_canceled)
      return;
    // If we're not frozen anymore, the task must have ended in the same asio
    // poll than the timeout: Thread::_wake was just called. Ignore the timeout.
    if (state() != state::frozen)
      return;
    ELLE_TRACE("%s: timed out", *this);
    this->_timeout = true;
    // FIXME: bug hunting, remove
    if (this->_waited.size() == 1 &&
        dynamic_cast<reactor::http::Request*>(*this->_waited.begin()))
      ELLE_WARN("DEBUG: timeout on HTTP request: %s", this->_waited);
    this->_wait_abort(elle::sprintf("wait timeout for %s (waiting %s)",
                                    waited, this->_waited));
  }

  void
  Thread::_wait_abort(std::string const& reason)
  {
    ELLE_TRACE("%s: abort wait because: %s", *this, reason);
    ELLE_ASSERT_EQ(state(), state::frozen);
    BOOST_FOREACH (Waitable* waitable, _waited)
      waitable->_unwait(this);
    this->_waited.clear();
    this->_timeout_timer.cancel();
    this->_scheduler._unfreeze(*this, reason);
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
  Thread::_wake(Waitable* waitable)
  {
    ELLE_TRACE("%s: wait ended for %s", *this, *waitable)
    {
      if (waitable->_exception && !this->_exception)
      {
        ELLE_TRACE("%s: forward exception", *this);
        this->_exception = waitable->_exception;
      }
      this->_waited.erase(waitable);
      if (this->_waited.empty())
      {
        ELLE_TRACE("%s: nothing to wait on, waking up", *this);
        this->_scheduler._unfreeze(
          *this, elle::sprintf("wait for %s ended", *waitable));
        this->_state = Thread::state::running;
      }
      else
        ELLE_TRACE("%s: still waiting for %s other elements",
                   *this, this->_waited.size());
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

  void
  Thread::raise(std::exception_ptr e)
  {
    this->_exception = e;
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
