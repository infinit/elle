#include <elle/log.hh>

#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

#include <boost/foreach.hpp>

ELLE_LOG_COMPONENT("Reactor.Scheduler");

namespace reactor
{
  /*-------------.
  | Construction |
  `-------------*/

  Scheduler::Scheduler()
    : _current(0)
    , _starting()
    , _starting_mtx()
    , _running()
    , _frozen()
    , _io_service()
    , _io_service_work(new boost::asio::io_service::work(_io_service))
    , _manager()
  {}

  /*----.
  | Run |
  `----*/

  void
  Scheduler::run()
  {
    while (step())
      /* nothing */;
    delete _io_service_work;
    _io_service_work = 0;
    _io_service.run();
    ELLE_TRACE("Scheduler: done");
    assert(_frozen.empty());
  }

  bool
  Scheduler::step()
  {
    // Could avoid locking if no jobs are pending with a boolean.
    {
      boost::unique_lock<boost::mutex> lock(_starting_mtx);
      _running.insert(_starting.begin(), _starting.end());
      _starting.clear();
    }
    Threads running(_running);
    ELLE_TRACE("Scheduler: new round with %s jobs", running.size());
    BOOST_FOREACH (Thread* t, running)
    {
      ELLE_TRACE("Scheduler: schedule %s", *t);
      _step(t);
    }
    ELLE_TRACE("Scheduler: run asio callbacks");
    _io_service.reset();
    _io_service.poll();
    if (_running.empty() && _starting.empty())
      {
        if (_frozen.empty())
          return false;
        else
          while (_running.empty() && _starting.empty())
            {
              ELLE_TRACE("Scheduler: nothing to do, "
                                    "polling asio in a blocking fashion");
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
            }
      }
    return true;
  }

  void
  Scheduler::_step(Thread* thread)
  {
    Thread* previous = _current;
    _current = thread;
    try
      {
        thread->_step();
      }
    catch (const std::runtime_error& err)
      {
        std::cerr << "thread " << thread->name() << ": "
                  << err.what() << std::endl;
        std::abort();
      }
    catch (...)
      {
        std::cerr << "thread " << thread->name() << ": "
                  << "unknown error" << std::endl;
        std::abort();
      }
    _current = previous;
    if (thread->state() == Thread::state::done)
      {
        ELLE_TRACE("Scheduler: cleanup %s", *thread);
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
    assert(thread.state() == Thread::state::running);
    assert(_running.find(&thread) != _running.end());
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
    assert(thread.state() == Thread::state::frozen);
    _frozen.erase(&thread);
    _running.insert(&thread);
    if (_running.size() == 1)
      _io_service.post(nothing);
  }

  void
  Scheduler::terminate()
  {
    BOOST_FOREACH(Thread* t, _starting)
      if (t->_dispose)
        {
          // Threads expect to be done when deleted. For this very
          // particuliar case, hack the state before deletion.
          t->_state = Thread::state::done;
          delete t;
        }
    _starting.clear();
    BOOST_FOREACH(Thread* t, Threads(_running))
      if (t != _current)
        t->terminate();
    BOOST_FOREACH(Thread* t, Threads(_frozen))
      t->terminate();
  }

  void
  Scheduler::_terminate(Thread* thread)
  {
    ELLE_LOG_COMPONENT("Reactor.Thread");
    ELLE_TRACE("%s: terminate", *thread);
    if (current() == thread)
      throw Terminate(*this);
    // If the underlying coroutine was never run, nothing to do.
    if (_starting.erase(thread))
      {
        thread->_state = Thread::state::done;
        return;
      }
    switch (thread->state())
      {
        case Thread::state::running:
          thread->raise(new Terminate(*this));
          break;
        case Thread::state::frozen:
          thread->raise(new Terminate(*this));
          thread->_wait_abort();
          break;
        case Thread::state::done:
          break;
      }
  }

  void
  Scheduler::_terminate_now(Thread* thread)
  {
    _terminate(thread);
    if (thread->state() != Thread::state::done)
      {
        _step(thread);
        assert(thread->state() == Thread::state::done);
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

  static void EveryHelper(Scheduler* sched,
                              const boost::function<void ()>& f,
                              Duration delay)
  {
    while (true)
      {
        sched->current()->sleep(delay);
        f();
      }
  }

  void
  Scheduler::Every(const boost::function<void ()>&  f,
                   const std::string&               name,
                   Duration                         delay)
  {
    new Thread(*this, name,
               boost::bind(&EveryHelper, this, f, delay), true);
  }

  /*-----.
  | Asio |
  `-----*/

  boost::asio::io_service&
  Scheduler::io_service()
  {
    return _io_service;
  }
}
