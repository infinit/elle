#include <elle/finally.hh>
#include <elle/log.hh>
#include <elle/assert.hh>

#include <reactor/exception.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

#include <boost/foreach.hpp>

ELLE_LOG_COMPONENT("reactor.Scheduler");

namespace reactor
{
  /*-------------.
  | Construction |
  `-------------*/

  Scheduler::Scheduler():
    _done(false),
    _shallstop(false),
    _current(0),
    _starting(),
    _starting_mtx(),
    _running(),
    _frozen(),
    _io_service(),
    _io_service_work(new boost::asio::io_service::work(_io_service)),
    _manager()
  {
    this->_eptr = nullptr;
  }

  /*------------------.
  | Current Scheduler |
  `------------------*/

  static
  std::unordered_map<std::thread::id, Scheduler*>&
  _schedulers()
  {
    static std::unordered_map<std::thread::id, Scheduler*> map;
    return map;
  }

  static
  std::mutex&
  _schedulers_mutex()
  {
    static std::mutex mutex;
    return mutex;
  }

  Scheduler*
  Scheduler::scheduler()
  {
    std::unique_lock<std::mutex> ulock(_schedulers_mutex());
    return _schedulers()[std::this_thread::get_id()];
  }

  static
  void
  scheduler(Scheduler* v)
  {
    std::unique_lock<std::mutex> ulock(_schedulers_mutex());
    _schedulers()[std::this_thread::get_id()] = v;
  }

  /*----.
  | Run |
  `----*/

  void
  Scheduler::run()
  {
    reactor::scheduler(this);
    ELLE_TRACE_SCOPE("%s: run", *this);
    reactor::scheduler(nullptr);
    ELLE_ASSERT(!scheduler());
    while (step())
      /* nothing */;
    delete _io_service_work;
    _io_service_work = 0;
    _io_service.run();
    reactor::scheduler(this);
    _done = true;
    ELLE_TRACE("%s: done", *this);
    reactor::scheduler(nullptr);
    ELLE_ASSERT(_frozen.empty());
    if (_eptr != nullptr)
    {
      std::rethrow_exception(_eptr);
    }
  }

  bool
  Scheduler::step()
  {
    reactor::scheduler(this);
    ELLE_FINALLY(pop, reactor::scheduler(nullptr));

    // Could avoid locking if no jobs are pending with a boolean.
    {
      boost::unique_lock<boost::mutex> lock(_starting_mtx);
      auto& ordered = this->_starting.get<1>();
      _running.insert(ordered.begin(), ordered.end());
      _starting.clear();
    }
    auto& ordered = this->_running.get<1>();
    std::vector<Thread*> running(ordered.begin(), ordered.end());
    ELLE_TRACE("Scheduler: new round with %s jobs", running.size());
    BOOST_FOREACH (Thread* t, running)
    {
      // If the thread was stopped during this round, skip. Can be caused by
      // terminate_now, for instance.
      if (_running.find(t) == _running.end())
        continue;
      ELLE_TRACE("Scheduler: schedule %s", *t);
      _step(t);
    }
    ELLE_TRACE("%s: run asynchronous jobs", *this)
    {
      try
      {
        _io_service.reset();
        _io_service.poll();
      }
      catch (std::exception const& e)
      {
        ELLE_WARN("%s: asynchronous jobs threw an exception: %s",
                  *this, e.what());
        this->_eptr = std::current_exception();
        this->terminate();
      }
      catch (...)
      {
        ELLE_WARN("%s: asynchronous jobs threw an unknown exception", *this);
        this->_eptr = std::current_exception();
        this->terminate();
      }
    }
    if (_running.empty() && _starting.empty())
    {
      if (_frozen.empty())
      {
        ELLE_TRACE_SCOPE("%s: no threads left, we're done", *this);
        return false;
      }
      else
        while (_running.empty() && _starting.empty())
        {
          ELLE_TRACE("%s: nothing to do, "
                     "polling asio in a blocking fashion", *this);
          _io_service.reset();
          boost::system::error_code err;
          std::size_t run = _io_service.run_one(err);
          if (err)
          {
            std::cerr << "fatal ASIO error: " << err << std::endl;
            std::abort();
          }
          else if (run == 0)
          {
            std::cerr << "ASIO service is dead." << std::endl;
            std::abort();
          }
          if (this->_shallstop)
            break;
        }
    }
    else
      ELLE_TRACE("%s: round end with active threads", *this);
    if (this->_shallstop)
      this->terminate();
    return true;
  }

  void
  Scheduler::_step(Thread* thread)
  {
    ELLE_ASSERT_EQ(thread->state(), Thread::State::running);
    Thread* previous = _current;
    _current = thread;
    try
    {
      thread->_step();
      _current = previous;
    }
    catch (...)
    {
      _current = previous;
      ELLE_ERR("%s: exception escaped, terminating: %s",
               *this, elle::exception_string())
        this->terminate();
      this->_eptr = std::current_exception();
    }
    if (thread->state() == Thread::state::done)
    {
      ELLE_TRACE("%s: %s finished", *this, *thread);
      _running.erase(thread);
      if (thread->_dispose)
        delete thread;
    }
  }

  /*-------------------.
  | Threads management |
  `-------------------*/

  Thread*
  Scheduler::current() const
  {
    return _current;
  }

  void
  Scheduler::_freeze(Thread& thread)
  {
    ELLE_ASSERT_EQ(thread.state(), Thread::state::running);
    ELLE_ASSERT_NEQ(_running.find(&thread), _running.end());
    _running.erase(&thread);
    _frozen.insert(&thread);
  }

  static void
  nothing()
  {}

  void
  Scheduler::_thread_register(Thread& thread)
  {
    // FIXME: be thread safe only if needed
    {
      boost::unique_lock<boost::mutex> lock(_starting_mtx);
      _starting.insert(&thread);
      // Wake the scheduler.
      _io_service.post(nothing);
    }
  }

  void
  Scheduler::_unfreeze(Thread& thread)
  {
    ELLE_ASSERT_EQ(thread.state(), Thread::state::frozen);
    _frozen.erase(&thread);
    _running.insert(&thread);
    if (_running.size() == 1)
      _io_service.post(nothing);
  }

  void
  Scheduler::terminate_later()
  {
    ELLE_TRACE_METHOD("");
    this->_shallstop = true;
    this->io_service().post([&] {});
  }

  Scheduler::Threads
  Scheduler::terminate()
  {
    ELLE_TRACE_SCOPE("%s: terminate", *this);
    Threads terminated;
    BOOST_FOREACH(Thread* t, _starting)
    {
      // Threads expect to be done when deleted. For this very
      // particuliar case, hack the state before deletion.
      t->_state = Thread::state::done;
      if (t->_dispose)
        delete t;
    }
    _starting.clear();
    BOOST_FOREACH(Thread* t, Threads(_running))
      if (t != _current)
      {
        t->terminate();
        terminated.insert(t);
      }
    BOOST_FOREACH(Thread* t, Threads(_frozen))
    {
      t->terminate();
      terminated.insert(t);
    }
    return terminated;
  }

  void
  Scheduler::terminate_now()
  {
    auto threads = this->terminate();
    this->current()->wait(Waitables(threads.begin(), threads.end()));
  }

  void
  Scheduler::_terminate(Thread* thread)
  {
    ELLE_TRACE_SCOPE("%s: terminate %s", *this, *thread);
    if (current() == thread)
      throw Terminate(thread->name());
    // If the underlying coroutine was never run, nothing to do.
    if (_starting.erase(thread))
      {
        thread->_state = Thread::state::done;
        return;
      }
    switch (thread->state())
      {
        case Thread::state::running:
          thread->raise<Terminate>(thread->name());
          break;
        case Thread::state::frozen:
          thread->raise<Terminate>(thread->name());
          thread->_wait_abort();
          ELLE_ASSERT_EQ(thread->state(), Thread::state::running);
          break;
        case Thread::state::done:
          break;
      }
  }

  void
  Scheduler::_terminate_now(Thread* thread)
  {
    _terminate(thread);
    this->current()->wait(*thread);
  }

  /*-------.
  | Status |
  `-------*/

  static void
  DumpSet(Scheduler::Threads const&     threads,
          std::ostream&                 s)
  {
    BOOST_FOREACH (Thread const* t, threads)
    {
      s << "    ";
      t->Dump(s);
      s << std::endl;
    }
  }

  void
  Scheduler::dump(std::ostream& s)
  {
    s << "Scheduler" << std::endl;
    DumpSet(_running, s);
    DumpSet(_frozen, s);
  }

  void
  Scheduler::debug()
  {
    dump(std::cerr);
  }

  /*----------.
  | Printable |
  `----------*/

  void
  Scheduler::print(std::ostream& s) const
  {
    s << "Scheduler " << this;
  }

  /*----------.
  | Shortcuts |
  `----------*/

  static void CallLaterHelper(Scheduler* sched,
                              const boost::function<void ()>& f,
                              Duration delay)
  {
    sched->current()->sleep(delay);
    f();
  }

  void
  Scheduler::CallLater(const boost::function<void ()>&  f,
                       const std::string&               name,
                       Duration                         delay)
  {
    new Thread(*this, name,
               boost::bind(&CallLaterHelper, this, f, delay), true);
  }

  static void
  every_helper(Scheduler* sched,
               const boost::function<void ()>& f,
               Duration delay)
  {
    while (true)
    {
      sched->current()->sleep(delay);
      f();
    }
  }

  Thread*
  Scheduler::every(const boost::function<void ()>& f,
                   const std::string& name,
                   Duration delay,
                   bool dispose)
  {
    return new Thread(*this, name,
                      boost::bind(&every_helper, this, f, delay), dispose);
  }

  /*-----.
  | Asio |
  `-----*/

  boost::asio::io_service&
  Scheduler::io_service()
  {
    return _io_service;
  }

  /*----------------.
  | Multithread API |
  `----------------*/

  template <>
  void
  Scheduler::mt_run<void>(const std::string& name,
                          const boost::function<void ()>& action)
  {
    // Bounce on the non-void case with a dummy int value.
    this->mt_run<int>(name, [&] () { action(); return 42; });
  }
}
