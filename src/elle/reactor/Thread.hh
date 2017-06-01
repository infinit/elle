#pragma once

#include <boost/signals2.hpp>
#include <boost/system/error_code.hpp>

#include <elle/Backtrace.hh>
#include <elle/With.hh>
#include <elle/das/Symbol.hh>
#include <elle/das/named.hh>
#include <elle/reactor/asio.hh>
#include <elle/reactor/backend/fwd.hh>
#include <elle/reactor/duration.hh>
#include <elle/reactor/fwd.hh>
#include <elle/reactor/signals.hh>
#include <elle/reactor/Waitable.hh>

namespace elle
{
  namespace reactor
  {
    using ThreadPtr = std::shared_ptr<Thread>;

    ELLE_DAS_SYMBOL(dispose);
    ELLE_DAS_SYMBOL(managed);

    /// Thread represent a coroutine in a Scheduler environment.
    ///
    /// A Thread is a sequence of instructions to be executed. In the
    /// non-preemptive environment, a Thread will execute its instructions until
    /// it yields (via Thread::yield)  or waits (Waitable::wait), allowing other
    /// coroutines to execute.
    /// The Scheduler is in charge of managing Thread activity.
    /// A Thread lives until it's Thread::Action is over, the Thread is
    /// terminated by the Scheduler or an Exception escapes from Thread::Action.
    ///
    /// Thread can spawn other Threads, wait on Waitables, throw execptions that
    /// that can be catched or propagate to the parent Thread until it escapes
    /// the Scheduler and interrupt the cycles.
    ///
    /// A frequent pattern to be in a scheduler environment is:
    ///
    /// @code{.cc}
    ///
    /// int
    /// main(int main (int argc, char *argv[])
    /// {
    ///   auto sheduler = reactor::Scheduler{};
    ///   auto main_thread = reactor::Thread{
    ///      scheduler, "main",
    ///      []
    ///      {
    ///        // code to execute, e.g.:
    ///        // _main(argc, argv);
    ///      }};
    ///   try
    ///   {
    ///      scheduler.run();
    ///      return 0;
    ///   }
    ///   catch (std::runtime_error const&)
    ///   {
    ///      // do something.
    ///      return -1;
    ///   }
    /// }
    ///
    /// @endcode
    class Thread:
      public Waitable
    {
    /*------.
    | Types |
    `------*/
    public:
      using Self = Thread;
      using Action = backend::Action;
      class Terminator
        : public std::default_delete<reactor::Thread>
      {
      public:
        void
        operator ()(reactor::Thread* t);
      };
      friend class Terminator;
      class unique_ptr
        : public std::unique_ptr<reactor::Thread, Terminator>
      {
      public:
        template <typename ... Args>
        unique_ptr(Args&& ... args);
      private:
        boost::signals2::scoped_connection _slot;
      };

    /*-------------.
    | Construction |
    `-------------*/
    public:
      /// Create a Thread that will run an Action, using the given Scheduler.
      ///
      /// @param scheduler The Scheduler in charge of the Thread.
      /// @param name A descriptive name of Thread to be spawn.
      /// @param action The Action to execute.
      /// @param dispose Put the Scheduler in charge of destroying the Thread.
      Thread(Scheduler& scheduler,
             const std::string& name,
             Action action,
             bool dispose = false);
      /// Create a Thread that will run an Action, using the current Scheduler.
      ///
      /// @pre Must be in a invoked from another Thread.
      ///
      /// @param scheduler The Scheduler in charge of the Thread.
      /// @param name A descriptive name of Thread to be spawn.
      /// @param action The action to execute.
      /// @param dispose Put the Scheduler in charge of destroying the Thread.
      Thread(const std::string& name,
             Action action,
             bool dispose = false);
      /// Create a Thread that will run an Action, using the current Scheduler.
      ///
      /// @pre Must be in a invoked from another Thread.
      ///
      /// @param scheduler The Scheduler in charge of the Thread.
      /// @param name A descriptive name of Thread to be spawn.
      /// @param action The action to execute.
      /// @param args The named arguments `dispose` and `managed`.
      template <typename ... Args>
      Thread(const std::string& name,
             Action action,
             Args&& ... args);

      /// Create a Thread.
      ///
      /// Returned std::shared_ptr will be kept live until action finished.
      ///
      /// @param scheduler The Scheduler in charge of managing the Thread.
      /// @param name A descriptive name.
      /// @param action The Action to perform.
      /// @returns A std::shared_ptr managing the created Thread.
      static
      ThreadPtr
      make_tracked(Scheduler& scheduler,
                   const std::string& name,
                   Action action);
      /// Create a Thread.
      ///
      /// Same as make_tracked(Scheduler&, ...) but uses the current Scheduler.
      /// See reactor::Scheduler::scheduler() for more details.
      ///
      /// @param name A descriptive name.
      /// @param action The Action to perform.
      /// @returns A std::shared_ptr managing the created Thread.
      static
      ThreadPtr
      make_tracked(const std::string& name,
                   Action action);
      virtual
      ~Thread();
    protected:
      /// Called by the scheduler when it doesn't reference this anymore.
      virtual
      void
      _scheduler_release();
    private:
      ELLE_ATTRIBUTE(ThreadPtr, self);
      ELLE_ATTRIBUTE_RW(bool, dispose);
      ELLE_ATTRIBUTE_RW(bool, managed);

    /*----------.
    | Backtrace |
    `----------*/
    public:
      elle::Backtrace
      backtrace() const;
      ELLE_ATTRIBUTE(elle::Backtrace, yield_backtrace);

      /*---------.
      | Tracking |
      `---------*/
    public:
      using Tracker = boost::signals2::signal<void ()>;
    private:
      /// signal invoked when Thread object is being destroyed.
      ELLE_ATTRIBUTE_X(Tracker, destructed);
      // signal invoked when Thread is released by the Scheduler.
      ELLE_ATTRIBUTE_X(Tracker, released);

    /*-------.
    | Status |
    `-------*/
    public:
      /// State represents every possible state a Thread can be in.
      enum class State
      {
        /// The thread has finished.
        done,
        /// The thread is currently running.
        running,
        /// The thread is blocked on some condition or sleeping.
        frozen,
      };
      /// Current state.
      ELLE_ATTRIBUTE_R(State, state);
      /// Whether our state is 'State::done'.
      bool
      done() const;
      /// Pretty name.
      std::string
      name() const;
    /*----------.
    | Printable |
    `----------*/
    public:
      void
      print(std::ostream& output) const override;

      /*----.
      | Run |
      `----*/
    public:
      /// Sleep for a certain duration.
      ///
      /// @param d The duration the Thread will sleep.
      void
      sleep(Duration d);
      /// Yield, allowing other Threads to be executed without changing the
      /// current state.
      void
      yield();
    private:
      void
      _step();

    /*-----------.
    | Exceptions |
    `-----------*/
    public:
      using Injection = std::function<void ()>;
      void
      inject(const Injection& injection);
      template <typename Exception, typename... Args>
      void
      raise(Args&&... args);
      void
      raise(std::exception_ptr e);
      template <typename Exception, typename... Args>
      void
      raise_and_wake(Args&&... args);
      void
      raise_and_wake(std::exception_ptr e);
    protected:
      virtual
      void
      _action_wrapper(const Thread::Action& action);
    protected:
      std::function<void ()> _injection;
      /// Exception to be thrown by the thread upon wake-up.
      std::exception_ptr _exception;
      friend class Exception;
      /// Exception thrown by the thread execution.
      std::exception_ptr _exception_thrown;

    /*----------------.
    | Synchronization |
    `----------------*/
    public:
      /// Wait for a group waitables to be done.
      ///
      /// If any of the specified waitables asks to be waited for, this will
      /// freeze the thread until all of them are done. A timeout may be
      /// specified, in which case the wait will be aborted and false will be
      /// returned.
      ///
      /// @see Waitable.
      ///
      /// @param waitables The Waitables to wait for.
      /// @param timeout The maximum delay before aborting the operation.
      /// @returns Whether the operation finished and didn't timeout.
      bool
      wait(Waitables const& waitables,
           DurationOpt timeout = DurationOpt());
      /// Shortcut to wait for a single waitable to be done.
      bool
      wait(Waitable& s,
           DurationOpt timeout = DurationOpt());
      /// Terminate execution of the thread by injecting a terminate exception.
      void
      terminate();
      /// Terminate thread and wait for termination to be effective.
      ///
      /// @param suicide If true, terminate_now is a noop if this thread is
      ///                the current one.
      void
      terminate_now(bool suicide = true);
    protected:
      bool
      _wait(Thread* thread, Waker const& waker) override;
    private:
      friend class Scope;
      friend class TimeoutGuard;
      friend class Waitable;
      void
      _wait_timeout(const boost::system::error_code& e,
                    std::string const& waited);
      void
      _wait_abort(std::string const& reason);
      void
      _freeze();
      void
      _wake(Waitable* waitable);
      ELLE_ATTRIBUTE_R(std::set<Waitable*>, waited);
      ELLE_ATTRIBUTE(bool, timeout);
      ELLE_ATTRIBUTE(boost::asio::deadline_timer, timeout_timer);

    /*------.
    | Hooks |
    `------*/
    public:
      ELLE_ATTRIBUTE_RX(boost::signals2::signal<void ()>, frozen);
      ELLE_ATTRIBUTE_RX(
        boost::signals2::signal<void (std::string const&)>, unfrozen);

    /*---------.
    | Contexts |
    `---------*/
    public:
      /// Change properties on current thread and restore upon destruction.
      class Context
      {
      protected:
        Context();
        ~Context() noexcept(false);
        friend class elle::With<Context>;
        ELLE_ATTRIBUTE(Thread&, current);
        ELLE_ATTRIBUTE(bool, interruptible);

      public:
        bool
        interruptible(bool interruptible);
      };

      /// Marks current thread noniterruptible while this object lives.
      class NonInterruptible
        : public Context
      {
      protected:
        NonInterruptible();
        friend class elle::With<NonInterruptible>;
      };

      /// Mark thread interrutible for the lifetime of this object.
      class Interruptible
        : public Context
      {
      protected:
        Interruptible();
        friend class elle::With<Interruptible>;
      };

    /*--------.
    | Backend |
    `--------*/
    public:
      Scheduler& scheduler();
    private:
      friend class Scheduler;
      ELLE_ATTRIBUTE(std::unique_ptr<backend::Thread>, thread);
      ELLE_ATTRIBUTE(Scheduler&, scheduler);
      ELLE_ATTRIBUTE_R(bool, terminating);
      /// If set to false, do not rethrow Terminate exception.
      ELLE_ATTRIBUTE_Rw(bool, interruptible);
    };

    template <typename R>
    class VThread: public Thread
    {
    /*---------.
    | Typedefs |
    `---------*/
    public:
      using Self = VThread<R>;
      using Action = std::function<R ()>;

      /*-------------.
      | Construction |
      `-------------*/
    public:
      VThread(Scheduler& scheduler,
              const std::string& name,
              Action action);

      /*-------.
      | Result |
      `-------*/
    public:
      const R&
      result() const;
    private:
      R _result;
    };

    /*----------------.
    | Print operators |
    `----------------*/

    std::ostream& operator << (std::ostream& s, Thread::State state);

    /*------.
    | Every |
    `------*/

    /// Run an operation at a given frequency.
    ///
    /// Run the @param op operation every @param freq. For now, the
    /// implementation simply sleeps for @param freq between every action thus
    /// inducing a drift. This is suitable for actions that need to be run
    /// in background roughly regularly such as garbage collecting,
    /// notification checking, ... It is not suitable if the frequency must be
    /// exactly met on the long term.
    ///
    /// The thread that run the operation is returned, enabling the caller to
    /// stop it at will.
    ///
    /// @param freq The frequency at which to run the operation.
    /// @param op The Operation to run.
    /// @param name The Name of the thread that will run the operation.
    /// @param dispose Whether to delete the thread when it's done.
    /// @returns The thread running the operation.
    Thread::unique_ptr
    every(Duration freq,
          const std::string& name,
          const std::function<void ()>& op,
          bool dispose = false);
  }
}

#include <elle/reactor/Thread.hxx>
