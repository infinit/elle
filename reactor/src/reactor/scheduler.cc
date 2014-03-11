#ifdef REACTOR_PYTHON_BINDING
# include <boost/python.hpp>
# include <reactor/python.hh>
#endif

#include <elle/Measure.hh>
#include <elle/attribute.hh>
#include <elle/assert.hh>
#include <elle/finally.hh>
#include <elle/log.hh>
#include <elle/memory.hh>

#include <reactor/exception.hh>
#include <reactor/operation.hh>
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
    _background_service(),
    _background_service_work(
      new boost::asio::io_service::work(this->_background_service)),
    _background_pool(),
    _background_pool_free(0),
    _io_service(),
    _io_service_work(new boost::asio::io_service::work(this->_io_service)),
    _manager(),
    _running_thread()
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

  class PushScheduler
  {
  public:
    PushScheduler(Scheduler* sched):
      _previous(Scheduler::scheduler())
    {
      reactor::scheduler(sched);
    }

    ~PushScheduler()
    {
      reactor::scheduler(this->_previous);
    }

    ELLE_ATTRIBUTE(Scheduler*, previous);
  };

  void
  Scheduler::run()
  {
    {
      PushScheduler p(this);
      ELLE_TRACE_SCOPE("%s: run", *this);
    }
    this->_running_thread = std::this_thread::get_id();
    while (step())
      /* nothing */;
    this->_running_thread = std::thread::id();
    delete this->_background_service_work;
    this->_background_service_work = 0;
    for (auto& thread: this->_background_pool)
      thread.join();
    delete _io_service_work;
    _io_service_work = 0;
    // Cancel all pending signal handlers.
    this->_signal_handlers.clear();
    _io_service.run();
    _done = true;
    {
      PushScheduler p(this);
      ELLE_TRACE("%s: done", *this);
    }
    ELLE_ASSERT(_frozen.empty());
    if (_eptr != nullptr)
    {
#ifdef REACTOR_PYTHON_BINDING
      try
      {
#endif
        std::rethrow_exception(_eptr);
#ifdef REACTOR_PYTHON_BINDING
      }
      catch (PythonException const& e)
      {
        e.restore();
      }
#endif
    }
  }

  static
  std::ostream&
  operator <<(std::ostream& output, Scheduler::Threads const& threads)
  {
    bool first = true;
    output << "[";
    for (auto thread: threads)
    {
      if (first)
        first = false;
      else
        output << ", ";
      output << *thread;
    }
    output << "]";
    return output;
  }

  bool
  Scheduler::step()
  {
    PushScheduler p(this);
    // Could avoid locking if no jobs are pending with a boolean.
    {
      boost::unique_lock<boost::mutex> lock(_starting_mtx);
      auto& ordered = this->_starting.get<1>();
      _running.insert(ordered.begin(), ordered.end());
      _starting.clear();
    }
    auto& ordered = this->_running.get<1>();
    std::vector<Thread*> running(ordered.begin(), ordered.end());
    ELLE_TRACE_SCOPE("Scheduler: new round with %s jobs", running.size());

    ELLE_DUMP("%s: starting: %s", *this, this->_starting);
    ELLE_DUMP("%s: running: %s", *this, this->_running);
    ELLE_DUMP("%s: frozen: %s", *this, this->_frozen);
    ELLE_MEASURE("Scheduler round")
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
      ELLE_MEASURE_SCOPE("Asio callbacks");
      try
      {
        _io_service.reset();
        auto n = _io_service.poll();
        ELLE_DEBUG("%s: %s callback called", *this, n);
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
          ELLE_TRACE_SCOPE("%s: nothing to do, "
                     "polling asio in a blocking fashion", *this);
          _io_service.reset();
          boost::system::error_code err;
          std::size_t run = _io_service.run_one(err);
          ELLE_DEBUG("%s: %s callback called", *this, run);
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
      ELLE_WARN("%s: exception escaped, terminating: %s",
                *this, elle::exception_string())
        this->terminate();
      this->_eptr = std::current_exception();
    }
    if (thread->state() == Thread::state::done)
    {
      ELLE_TRACE("%s: %s finished", *this, *thread);
      _running.erase(thread);
      thread->_scheduler_release();
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
      t->_scheduler_release();
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
    ELLE_ASSERT(this->current());
    this->current()->wait(Waitables(threads.begin(), threads.end()));
  }

  bool
  Scheduler::_terminate(Thread* thread)
  {
    ELLE_TRACE_SCOPE("%s: terminate %s", *this, *thread);
    if (current() == thread)
    {
      ELLE_DEBUG("%s: terminating the current thread %s, throwing directly",
                 *this, *thread);
      throw Terminate(thread->name());
    }
    // If the underlying coroutine was never run, nothing to do.
    if (_starting.erase(thread))
    {
      ELLE_DEBUG("%s: %s was starting, discard it", *this, *thread);
      thread->_state = Thread::state::done;
      thread->_scheduler_release();
      return true;
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
          return true;
          break;
      }
    return false;
  }

  void
  Scheduler::_terminate_now(Thread* thread,
                            bool suicide)
  {
    if (!suicide && this->current() == thread)
      return;

    bool ready = _terminate(thread);
    if (ready)
      return;

    // Wait on the thread object and ignore exceptions until the wait is done.
    std::exception_ptr saved_exception(nullptr);
    while (true)
    {
      try
      {
        this->current()->wait(*thread);
      }
      catch (...)
      {
        saved_exception = std::current_exception();
        ELLE_TRACE("%s: terminate_now interrupted, delaying exception: %s",
                   *this, elle::exception_string());
        continue;
      }

      break;
    }

    if (saved_exception != nullptr)
    {
      ELLE_TRACE("%s: terminate_now finished, re-throwing: %s",
                 *this, elle::exception_string(saved_exception));
      std::rethrow_exception(saved_exception);
    }
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

  void
  Scheduler::run_later(const std::string& name,
                       const std::function<void ()>&f)
  {
    new Thread(*this, name, f, true);
  }

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

  /*----------------.
  | Background jobs |
  `----------------*/

  int
  Scheduler::background_pool_size() const
  {
    return this->_background_pool.size();
  }

  class BackgroundOperation:
    public Operation
  {
  public:
    typedef std::function<void ()> Action;
    struct Status
    {
      bool aborted;
    };

  public:
    BackgroundOperation(Action const& action):
      Operation(*Scheduler::scheduler()),
      _action(action),
      _status(new Status)
    {
      this->_status->aborted = false;
    }
    ELLE_ATTRIBUTE(Action, action);
    std::shared_ptr<Status> _status;

  protected:
    virtual
    void
    _abort() override
    {
      auto& sched = *Scheduler::scheduler();
      ELLE_TRACE_SCOPE("%s: abort background operation", sched);
      this->_status->aborted = true;
      this->_signal();
      ++sched._background_pool_free;
    }

    virtual
    void
    _start() override
    {
      auto& sched = *Scheduler::scheduler();
      auto& current = *sched.current();
      if (sched._background_pool_free == 0)
      {
        ELLE_DEBUG("%s: spawn new background thread", sched);
        sched._background_pool.emplace_back([&sched]
                                            {
                                              sched._background_service.run();
                                            });
      }
      else
        --sched._background_pool_free;
      auto status = this->_status;
      auto action = this->_action;
      sched._background_service.post(
        [this, status, action, &current, &sched]
        {
          try
          {
            ELLE_TRACE_SCOPE("%s: run background operation", sched);
            action();
            ELLE_TRACE("%s: background operation finished", sched);
            sched.io_service().post([this, status, &sched]
                                    {
                                      if (!status->aborted)
                                      {
                                        this->_signal();
                                        ++sched._background_pool_free;
                                      }
                                    });
          }
          catch (...)
          {
            ELLE_TRACE("%s: background operation threw: %s",
                       sched, elle::exception_string());
            auto e = std::current_exception();
            sched.io_service().post([this, status, &sched, e]
                                    {
                                      if (!status->aborted)
                                      {
                                        this->_raise(e);
                                        this->_signal();
                                        ++sched._background_pool_free;
                                      }
                                    });
          }
      });
    }
  };

  void
  Scheduler::_run_background(std::function<void ()> const& action)
  {
    BackgroundOperation o(action);
    o.run();
  }

  /*--------.
  | Signals |
  `--------*/

  static
  void
  signal_callback(boost::asio::signal_set& set,
                  std::function<void ()> const& handler,
                  boost::system::error_code const& error,
                  int)
  {
    if (error == boost::asio::error::operation_aborted)
      return;
    handler();
    set.async_wait(std::bind(&signal_callback, std::ref(set), handler,
                             std::placeholders::_1, std::placeholders::_2));
  };

  namespace
  {
    std::string signal_string(int const signal)
    {
#ifdef INFINIT_WINDOWS
        //XXX should at least map common signal values
        return std::to_string(signal);
#else
        return ::strsignal(signal);
#endif
    }
  }

  void
  Scheduler::signal_handle(int signal, std::function<void ()> const& handler)
  {
    ELLE_LOG("%s: handle signal %s", *this, signal_string(signal));
    auto set = elle::make_unique<boost::asio::signal_set>(this->io_service(),
                                                          signal);
    set->async_wait(std::bind(&signal_callback, std::ref(*set), handler,
                              std::placeholders::_1, std::placeholders::_2));
    this->_signal_handlers.emplace_back(std::move(set));
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
    ELLE_ASSERT_NEQ(this->_running_thread, std::this_thread::get_id());
    // Bounce on the non-void case with a dummy int value.
    this->mt_run<int>(name, [&] () { action(); return 42; });
  }

  /*---------------.
  | Free functions |
  `---------------*/

  void
  background(std::function<void()> const& action)
  {
    auto* sched = Scheduler::scheduler();
    ELLE_ASSERT(sched);
    sched->_run_background(action);
  }

  void
  yield()
  {
    auto* sched = Scheduler::scheduler();
    ELLE_ASSERT(sched);
    auto* current = sched->current();
    ELLE_ASSERT(current);
    current->yield();
  }

  void
  sleep(Duration d)
  {
    auto* sched = Scheduler::scheduler();
    ELLE_ASSERT(sched);
    auto* current = sched->current();
    ELLE_ASSERT(current);
    current->sleep(d);
  }

  void
  sleep()
  {
    auto* sched = Scheduler::scheduler();
    ELLE_ASSERT(sched);
    auto* current = sched->current();
    ELLE_ASSERT(current);
    reactor::wait(*current);
  }

  bool
  wait(Waitable& waitable, DurationOpt timeout)
  {
    auto* sched = Scheduler::scheduler();
    ELLE_ASSERT(sched);
    auto* current = sched->current();
    ELLE_ASSERT(current);
    return current->wait(waitable, timeout);
  }

  bool
  wait(Waitables const& waitables, DurationOpt timeout)
  {
    auto* sched = Scheduler::scheduler();
    ELLE_ASSERT(sched);
    auto* current = sched->current();
    ELLE_ASSERT(current);
    return current->wait(waitables, timeout);
  }

  /** Run the given operation in the next cycle.
   *
   *  \param name Descriptive name of the operation, for debugging.
   *  \param f    Operation to run later.
   */
  void run_later(std::string const& name,
                 std::function<void ()> const& f)
  {
    auto* sched = Scheduler::scheduler();
    ELLE_ASSERT(sched);
    sched->run_later(name, f);
  }
}
