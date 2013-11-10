#ifndef INFINIT_REACTOR_THREAD_HH
# define INFINIT_REACTOR_THREAD_HH

# include <boost/asio.hpp>
# include <boost/optional.hpp>
# ifndef Q_MOC_RUN
#  include <boost/signal.hpp>
# endif
# include <boost/system/error_code.hpp>

# include <elle/Backtrace.hh>

# include <reactor/backend/thread.hh>
# include <reactor/duration.hh>
# include <reactor/fwd.hh>
# include <reactor/waitable.hh>

namespace reactor
{
  class Thread: public Waitable
  {
    /*---------.
    | Typedefs |
    `---------*/
    public:
      typedef backend::Action Action;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      Thread(Scheduler& scheduler,
             const std::string& name,
             const Action& action,
             bool dispose = false);
      virtual
      ~Thread();
    protected:
      /// Called by the scheduler when it doesn't reference this anymore.
      virtual
      void
      _scheduler_release();
    private:
      bool _dispose;

    /*---------.
    | Tracking |
    `---------*/
    public:
      typedef boost::signal<void ()> Tracker;
      Tracker& destructed();
    private:
      Tracker _destructed;

    /*-------.
    | Status |
    `-------*/
    public:
      struct state
      {
          enum State
          {
            /// The thread has finished.
            done,
            /// The thread is currently running.
            running,
            /// The thread is blocked on some condition or sleeping.
            frozen,
          };
      };
      typedef state::State State;
      /// Current state.
      State state() const;
      /// Whether our state is 'done'.
      bool done() const;
      /// Pretty name.
      std::string name() const;
      /// Debug dump.
      void Dump(std::ostream& output) const;
    private:
      State _state;

  /*----------.
  | Printable |
  `----------*/
  public:
    virtual
    void
    print(std::ostream& output) const override;

    /*----.
    | Run |
    `----*/
    public:
      void sleep(Duration d);
      void yield();
    private:
      void _step();

  /*-----------.
  | Exceptions |
  `-----------*/
  public:
    typedef boost::function<void ()> Injection;
    void inject(const Injection& injection);
    template <typename Exception, typename... Args>
    void raise(Args&&... args);
  private:
    void _action_wrapper(const Thread::Action& action);
    boost::function<void ()> _injection;
    /// Exception to be thrown by the thread upon wake-up.
    std::exception_ptr _exception;
    elle::Backtrace _backtrace_root;
    friend class Exception;
    /// Exception thrown by the thread execution.
    std::exception_ptr _exception_thrown;

    /*----------------.
    | Synchronization |
    `----------------*/
    public:
      /** Wait for a group waitables to be done.
       *
       * If any of the specified waitables asks to be waited for,
       * this will freeze the thread until all of them are done. A
       * timeout may be specified, in which case the wait will be
       * aborted and false will be returned.
       *
       * @see Waitable.
       *
       * @param waitables The waitables to wait for.
       * @param timeout Maximum delay before aborting the operation.
       * @return Whether the operation finished and didn't timeout.
       */
      bool wait(Waitables const& waitables,
                DurationOpt timeout = DurationOpt());
      /// Shortcut to wait for a single waitable to be done.
      bool wait(Waitable& s,
                DurationOpt timeout = DurationOpt());
      /** Terminate execution of the thread by injecting a terminate exception.
       *
       *
       */
      void terminate();
      void terminate_now(bool suicide = true);
    protected:
      virtual bool _wait(Thread* thread);
    private:
      friend class Waitable;
      void _wait_timeout(const boost::system::error_code& e);
      void _wait_abort();
      void _freeze();
      void _wake(Waitable* waitable);
      std::set<Waitable*> _waited;
      bool _timeout;
    boost::asio::deadline_timer _timeout_timer;

    /*--------.
    | Backend |
    `--------*/
  protected:
    Scheduler& scheduler();
  private:
    friend class Scheduler;
    backend::Thread _thread;
    Scheduler& _scheduler;
  };

  template <typename R>
  class VThread: public Thread
  {
  /*---------.
  | Typedefs |
  `---------*/
  public:
    typedef VThread<R> Self;
    typedef boost::function<R ()> Action;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      VThread(Scheduler& scheduler,
              const std::string& name,
              const Action& action);

    /*-------.
    | Result |
    `-------*/
    public:
      const R& result() const;
    private:
      R _result;
  };

  /*----------------.
  | Print operators |
  `----------------*/

  std::ostream& operator << (std::ostream& s, Thread::State state);
}

# include <reactor/thread.hxx>

#endif
