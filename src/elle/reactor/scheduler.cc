#include <elle/Measure.hh>
#include <elle/Plugin.hh>
#include <elle/assert.hh>
#include <elle/attribute.hh>
#include <elle/finally.hh>
#include <elle/log.hh>
#include <elle/make-vector.hh>
#include <elle/memory.hh>
#include <elle/os/environ.hh>
#include <elle/reactor/BackgroundOperation.hh>
#include <elle/reactor/backend/backend.hh>
#if defined REACTOR_CORO_BACKEND_IO
# include <elle/reactor/backend/coro_io/backend.hh>
#elif defined REACTOR_CORO_BACKEND_BOOST_CONTEXT
# include <elle/reactor/backend/boost_context/backend.hh>
#endif
#include <elle/reactor/exception.hh>
#include <elle/reactor/Operation.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/reactor/Thread.hh>

ELLE_LOG_COMPONENT("elle.reactor.Scheduler");

namespace
{
  auto const DBG = elle::os::inenv("REACTOR_SCHEDULER_DEBUG");
}

namespace elle
{
  namespace reactor
  {
    namespace plugins
    {
      extern elle::Plugin<elle::log::Indenter> logger_indentation;
      extern elle::Plugin<elle::log::Tag> logger_tags;
    }

    /*-------------.
    | Construction |
    `-------------*/

    Scheduler::Scheduler()
      : _done(false)
      , _shallstop(false)
      , _current(nullptr)
      , _starting()
      , _starting_mtx()
      , _running()
      , _frozen()
      , _background_service()
      , _background_service_work(
          new boost::asio::io_service::work(this->_background_service))
      , _background_pool()
      , _background_pool_free(0)
      , _io_service()
      , _io_service_work(new boost::asio::io_service::work(this->_io_service))
#if defined(REACTOR_CORO_BACKEND_IO)
      , _manager(new backend::coro_io::Backend())
#elif defined(REACTOR_CORO_BACKEND_BOOST_CONTEXT)
      , _manager(new backend::boost_context::Backend())
#else
# error "REACTOR_CORO_BACKEND not defined"
#endif
      , _running_thread()
    {
      this->_eptr = nullptr;
      plugins::logger_indentation.load();
      plugins::logger_tags.load();
#ifndef INFINIT_WINDOWS
      // Thread dumper on SIGUSR2.
      if (DBG)
        this->signal_handle(
          SIGUSR2,
          [&]
          {
            this->dump_state();
          });
#endif
    }

    Scheduler::~Scheduler()
    {
      delete this->_background_service_work;
      delete this->_io_service_work;
    }

    /*------------------.
    | Current Scheduler |
    `------------------*/

#ifdef __clang__

    namespace
    {
      auto&
      _schedulers()
      {
        static std::unordered_map<std::thread::id, Scheduler*> map;
        return map;
      }

      std::mutex&
      _schedulers_mutex()
      {
        static std::mutex mutex;
        return mutex;
      }

      void
      scheduler_set(Scheduler* v)
      {
        std::unique_lock<std::mutex> ulock(_schedulers_mutex());
        _schedulers()[std::this_thread::get_id()] = v;
      }
    }

    Scheduler*
    Scheduler::scheduler()
    {
      std::unique_lock<std::mutex> ulock(_schedulers_mutex());
      return _schedulers()[std::this_thread::get_id()];
    }

#else

    namespace
    {
      Scheduler*&
      _scheduler()
      {
        static thread_local Scheduler* sched = nullptr;
        return sched;
      }

      void
      scheduler_set(Scheduler* v)
      {
        _scheduler() = v;
      }
    }

    Scheduler*
    Scheduler::scheduler()
    {
      return _scheduler();
    }


#endif

    /*------.
    | Debug |
    `------*/

    void
    Scheduler::dump_state()
    {
      auto print_thread = [&] (reactor::Thread const& thread)
        {
          std::cerr << "  " << thread << ": " << thread.state();
          if (thread.terminating())
            std::cerr << " (terminating)";
          std::cerr << std::endl;
          std::cerr << "    waiting:" << std::endl;
          for (auto t: thread.waited())
            std::cerr << "      " << *t << std::endl;
          std::cerr << "    waiters:" << std::endl;
          for (auto t: thread.waiters())
            std::cerr << "      " << *(t.first) << std::endl;
          std::cerr << "    backtrace:" << std::endl;
          // FIXME: Indent the backtrace
          std::cerr << thread.backtrace() << std::endl;
        };
      if (!this->_frozen.empty())
      {
        std::cerr << "== FROZEN THREADS ==" << std::endl;
        for (auto thread: this->_frozen)
          print_thread(*thread);
      }
      if (!this->_running.empty())
      {
        std::cerr << "== RUNNING THREADS ==" << std::endl;
        for (auto thread: this->_running)
          print_thread(*thread);
      }
      if (!this->_starting.empty())
      {
        std::cerr << "== STARTING THREADS ==" << std::endl;
        for (auto thread: this->_starting)
          print_thread(*thread);
      }
    }

    /*----.
    | Run |
    `----*/

    class PushScheduler
    {
    public:
      PushScheduler(Scheduler* sched)
        : _previous(Scheduler::scheduler())
      {
        reactor::scheduler_set(sched);
      }

      ~PushScheduler()
      {
        reactor::scheduler_set(this->_previous);
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
      while (this->step())
        /* nothing */;
      this->_running_thread = std::thread::id();
      delete this->_background_service_work;
      this->_background_service_work = nullptr;
      for (auto& thread: this->_background_pool)
        thread.join();
      delete this->_io_service_work;
      this->_io_service_work = nullptr;
      // Cancel all pending signal handlers.
      this->_signal_handlers.clear();
      this->_io_service.run();
      this->_done = true;
      {
        PushScheduler p(this);
        ELLE_TRACE("%s: done", *this);
      }
      ELLE_ASSERT(this->_frozen.empty());
      if (this->_eptr != nullptr)
        this->_rethrow_exception(this->_eptr);
    }

    void
    Scheduler::_rethrow_exception(std::exception_ptr e) const
    {
      std::rethrow_exception(this->_eptr);
    }

    bool
    Scheduler::step()
    {
      PushScheduler p(this);
      // Could avoid locking if no jobs are pending with a boolean.
      {
        std::unique_lock<std::mutex> lock(this->_starting_mtx);
        auto& ordered = this->_starting.get<1>();
        this->_running.insert(ordered.begin(), ordered.end());
        this->_starting.clear();
      }
      auto& ordered = this->_running.get<1>();
      auto running = make_vector(ordered);
      ELLE_TRACE_SCOPE("Scheduler: new round with %s jobs", running.size());

      ELLE_DUMP("%s: starting: %s", *this, this->_starting);
      ELLE_DUMP("%s: running: %s", *this, this->_running);
      ELLE_DUMP("%s: frozen: %s", *this, this->_frozen);
      ELLE_MEASURE("Scheduler round")
        for (Thread* t: running)
        {
          // If the thread was stopped during this round, skip. Can be caused by
          // terminate_now, for instance.
          if (this->_running.find(t) == this->_running.end())
            continue;
          ELLE_TRACE("Scheduler: schedule %s", *t);
          this->_step(t);
        }
      ELLE_TRACE("%s: run asynchronous jobs", *this)
      {
        ELLE_MEASURE_SCOPE("Asio callbacks");
        try
        {
          this->_io_service.reset();
          auto n = this->_io_service.poll();
          ELLE_DEBUG("%s: %s callback called", *this, n);
        }
        catch (std::exception const& e)
        {
          ELLE_WARN("%s: asynchronous job threw an exception: %s",
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
      if (this->_running.empty() && this->_starting.empty())
      {
        if (this->_frozen.empty())
        {
          ELLE_TRACE_SCOPE("%s: no threads left, we're done", *this);
          return false;
        }
        else
          while (this->_running.empty() && this->_starting.empty())
          {
            ELLE_TRACE_SCOPE("%s: nothing to do, "
                       "polling asio in a blocking fashion", *this);
            this->_io_service.reset();
            boost::system::error_code err;
            std::size_t run = this->_io_service.run_one(err);
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
      Thread* previous = this->_current;
      this->_current = thread;
      try
      {
        thread->_step();
        this->_current = previous;
      }
      catch (...)
      {
        ELLE_LOG_LEVEL_SCOPE(
          DBG ? elle::log::Logger::Level::log : elle::log::Logger::Level::trace,
          DBG ? elle::log::Logger::Type::warning : elle::log::Logger::Type::info,
          "%s: exception escaped, terminating: %s",
          thread, elle::exception_string());
        this->_current = previous;
        this->_eptr = std::current_exception();
        this->terminate();
      }
      if (thread->state() == Thread::State::done)
      {
        ELLE_TRACE("%s: %s finished", *this, *thread);
        this->_running.erase(thread);
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
      ELLE_ASSERT_EQ(thread.state(), Thread::State::running);
      ELLE_ASSERT_NEQ(this->_running.find(&thread), this->_running.end());
      this->_running.erase(&thread);
      this->_frozen.insert(&thread);
      thread.frozen()();
    }

    namespace
    {
      void
      nothing()
      {}
    }

    void
    Scheduler::_thread_register(Thread& thread)
    {
      // FIXME: be thread safe only if needed
      {
        std::unique_lock<std::mutex> lock(this->_starting_mtx);
        this->_starting.insert(&thread);
        // Wake the scheduler.
        this->_io_service.post(nothing);
      }
    }

    void
    Scheduler::_unfreeze(Thread& thread, std::string const& reason)
    {
      ELLE_ASSERT_EQ(thread.state(), Thread::State::frozen);
      this->_frozen.erase(&thread);
      this->_running.insert(&thread);
      thread.unfrozen()(reason);
      if (this->_running.size() == 1)
        this->_io_service.post(nothing);
    }

    void
    Scheduler::terminate_later()
    {
      this->_shallstop = true;
      this->io_service().post([&] {});
    }

    Scheduler::Threads
    Scheduler::terminate()
    {
      ELLE_TRACE_SCOPE("%s: terminate", *this);
      Threads terminated;
      for(Thread* t: this->_starting)
      {
        // Threads expect to be done when deleted. For this very
        // particuliar case, hack the state before deletion.
        t->_state = Thread::State::done;
        t->_scheduler_release();
      }
      this->_starting.clear();
      for(Thread* t: Threads(this->_running))
        if (t != this->_current)
        {
          t->terminate();
          terminated.insert(t);
        }
      for(Thread* t: Threads(this->_frozen))
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
      if (this->_starting.erase(thread))
      {
        ELLE_DEBUG("%s: %s was starting, discard it", *this, *thread);
        thread->_state = Thread::State::done;
        thread->Waitable::_signal();
        thread->_scheduler_release();
        return true;
      }
      if (thread->_terminating)
        ELLE_DEBUG("%s: %s already terminating", *this, *thread);
      else
      {
        thread->_terminating = true;
        thread->raise<Terminate>(thread->name());
        if (thread->state() == Thread::State::frozen && thread->interruptible())
        {
          if (thread->state() == Thread::State::frozen)
          {
            thread->_wait_abort("thread termination");
            ELLE_ASSERT_EQ(thread->state(), Thread::State::running);
          }
        }
      }
      return thread->state() == Thread::State::done;
    }

    void
    Scheduler::_terminate_now(Thread* thread,
                              bool suicide)
    {
      if (!suicide && this->current() == thread)
        return;
      bool ready = this->_terminate(thread);
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
          ELLE_TRACE(
            "%s: terminatation of %s interrupted, delaying exception: %s",
            this, thread, elle::exception_string());
          continue;
        }
        break;
      }
      if (saved_exception != nullptr)
      {
        ELLE_TRACE("%s: termination of %s finished, re-throwing: %s",
                   this, thread, elle::exception_string(saved_exception));
        std::rethrow_exception(saved_exception);
      }
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

    namespace
    {
      void CallLaterHelper(Scheduler* sched,
                           const std::function<void ()>& f,
                           Duration delay)
      {
        sched->current()->sleep(delay);
        f();
      }
    }

    void
    Scheduler::CallLater(const std::function<void ()>&  f,
                         const std::string&               name,
                         Duration                         delay)
    {
      new Thread(*this, name,
                 std::bind(&CallLaterHelper, this, f, delay), true);
    }

    /*----------------.
    | Background jobs |
    `----------------*/

    int
    Scheduler::background_pool_size() const
    {
      return this->_background_pool.size();
    }

    void
    Scheduler::_run_background(std::function<std::function<void ()> ()> action)
    {
      // FIXME: don't hardcode 16, sort out CPU-bound versus "blocking" use case
      if (this->_background_pool_free == 0 && this->_background_pool.size() < 16)
      {
        ELLE_DEBUG("%s: spawn new background thread", *this);
        this->_background_pool.emplace_back(
          [this]
          {
            this->_background_service.run();
          });
      }
      else
        --this->_background_pool_free;
      this->_background_service.post(
        [this, action]
        {
          try
          {
            auto epilogue = elle::utility::move_on_copy(action());
            this->_io_service.post(
              [this, epilogue]
              {
                try
                {
                  (*epilogue)();
                  ++this->_background_pool_free;
                }
                catch (...)
                {
                  ++this->_background_pool_free;
                  throw;
                }
              });
          }
          catch (...)
          {
            ELLE_ABORT("background job threw: %s", elle::exception_string());
          }
        });
      }

    /*--------.
    | Signals |
    `--------*/

    namespace
    {
      void
      signal_callback(reactor::Scheduler& sched,
                      boost::asio::signal_set& set,
                      std::function<void ()> const& handler,
                      boost::system::error_code const& error,
                      int)
      {
        if (error == boost::asio::error::operation_aborted)
          return;
        new reactor::Thread(sched, "signal handler", handler, true);
        set.async_wait(std::bind(&signal_callback,
                                 std::ref(sched),
                                 std::ref(set),
                                 handler,
                                 std::placeholders::_1,
                                 std::placeholders::_2));
      }

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
      ELLE_TRACE_SCOPE("%s: handle signal %s", *this, signal_string(signal));
      auto set = std::make_unique<boost::asio::signal_set>(this->io_service(),
                                                            signal);
      set->async_wait(std::bind(&signal_callback,
                                std::ref(*this),
                                std::ref(*set),
                                handler,
                                std::placeholders::_1,
                                std::placeholders::_2));
      this->_signal_handlers.emplace_back(std::move(set));
    }

    /*-----.
    | Asio |
    `-----*/

    boost::asio::io_service&
    Scheduler::io_service()
    {
      return this->_io_service;
    }

    /*----------------.
    | Multithread API |
    `----------------*/

    template <>
    void
    Scheduler::mt_run<void>(const std::string& name,
                            const std::function<void ()>& action)
    {
      ELLE_ASSERT_NEQ(this->_running_thread, std::this_thread::get_id());
      // Bounce on the non-void case with a dummy int value.
      this->mt_run<int>(name, [&] () { action(); return 42; });
    }

    backend::Backend&
    Scheduler::manager()
    {
      return *this->_manager;
    }

    /*---------------.
    | Free functions |
    `---------------*/

    reactor::Scheduler&
    scheduler()
    {
      auto res = reactor::Scheduler::scheduler();
      if (!res)
        ELLE_ABORT("can't run outside a scheduler");
      return *res;
    }

    void
    background(std::function<void()> const& action)
    {
      BackgroundOperation<void> o(action);
      o.run();
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
      ELLE_TRACE("%s: sleep forever", current);
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

    bool
    wait(std::vector<std::reference_wrapper<Waitable>> const& waitables,
         DurationOpt timeout)
    {
      Waitables conv;
      conv.reserve(waitables.size());
      for (auto& w: waitables)
        conv.emplace_back(&w.get());
      return wait(conv, timeout);
    }

    void
    wait(boost::signals2::signal<void ()>& signal)
    {
      auto s = reactor::Signal{};
      boost::signals2::scoped_connection connection = signal.connect(
        [&] ()
        {
          s.signal();
        });
      reactor::wait(s);
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
}

/*----------------------------.
| Standdard library overrides |
`----------------------------*/

#if !defined INFINIT_LINUX && !defined INFINIT_ANDROID      \
    && !defined INFINIT_WINDOWS && !defined INFINIT_MACOSX  \
    && !defined INFINIT_IOS
# error "Unsupported platform"
#endif

#if defined(INFINIT_MACOSX)
// libc++
# include <elle/reactor/libcxx-exceptions/cxa_exception.hpp>
# define THROW_SPEC

#else
// libstdc++
# include <elle/reactor/libcxx-exceptions/unwind-cxx.h>
# define THROW_SPEC throw()

#endif

/* Override _cxa_get_globals from libc++/libstdc++
 * which uses TLS.
 * Add a per-scheduler-thread entry. Otherwise, std::current_exception leaks
 * between coroutines.
 */

using CXAThreadMap = std::unordered_map<std::thread::id,
  std::unique_ptr<__cxxabiv1::__cxa_eh_globals>>;

namespace
{
  CXAThreadMap&
  cxa_thread_map()
  {
    static CXAThreadMap _cxa_thread_map;
    return _cxa_thread_map;
  }
}

namespace __cxxabiv1
{
  extern "C"
  {
    __cxa_eh_globals *
    __cxa_get_globals() THROW_SPEC
    {
      // Always fetch the map to avoid static initialization fiascos.
      CXAThreadMap& map = cxa_thread_map();
      elle::reactor::Scheduler* sched = elle::reactor::Scheduler::scheduler();
      elle::reactor::backend::Thread* t = nullptr;
      if (sched != nullptr)
        t = sched->manager().current();
      if (sched == nullptr)
      {
        auto &res = map[std::this_thread::get_id()];
        if (!res)
          res.reset(new __cxa_eh_globals());
        return res.get();
      }
      if (t == nullptr)
      {
        static auto* nullthread_ceg = new __cxa_eh_globals();
        return nullthread_ceg;
      }
      auto* ceg = (__cxa_eh_globals*)t->exception_storage();
      return ceg;
    }

    __cxa_eh_globals *
    __cxa_get_globals_fast() THROW_SPEC
    {
      return __cxa_get_globals();
    }
  }
}
