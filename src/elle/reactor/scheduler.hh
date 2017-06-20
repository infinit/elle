#pragma once

#include <memory>
#include <mutex>
#include <thread>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#ifdef INFINIT_WINDOWS
# include <winsock2.h>
#endif
#include <boost/signals2.hpp>
#include <boost/thread.hpp>

#include <elle/Printable.hh>
#include <elle/attribute.hh>
#include <elle/reactor/asio.hh>
#include <elle/reactor/duration.hh>
#include <elle/reactor/fwd.hh>
#include <elle/reactor/backend/fwd.hh>

namespace elle
{
  namespace reactor
  {
    /// Sheduler is in charge of scheduling Threads (coroutines) execution.
    ///
    /// In the non-preemptive environment, coroutines can yield to allow other
    /// coroutines to be executed. The Scheduler is in charge of freezing the
    /// coroutine yielding, maintaining the context, choosing the coroutine to
    /// wake up, etc. In our scheduled environment, every single Thread is
    /// attached to a Scheduler.
    ///
    /// Entry point of the Scheduler is the Scheduler::run, which will start the
    /// cycles in charge of scheduling Threads activity. Scheduler::run will
    /// return when all Threads are over, if any of Scheduler::terminate* is
    /// called by by a Thread or if an exception escapes from any Thread
    /// (managed by this Scheduler).
    ///
    /// Scheduler::wait offers synchronization mechanisms via Waitable(s).
    /// A Thread (which inherits from Waitable) can be frozen and wait until a
    /// Waitable signals them to wake up.
    ///
    /// @code{.cc}
    ///
    /// auto sheduler = Scheduler{};
    /// auto t1 = Thread{
    ///   scheduler, "one",
    ///   []
    ///   {
    ///      std::cout << "1";
    ///      sheduler().yield();
    ///      std::cout << "2";
    ///      sheduler().yield();
    ///      std::cout << "3";
    ///   }};
    /// auto t2 = Thread{
    ///   scheduler, "two",
    ///   [&t1]
    ///   {
    ///      std::cout << "A";
    ///      t1.wait();
    ///      std::cout << "B";
    ///   }};
    /// // Scheduler::run will result on 1A23B.
    /// scheduler.run();
    /// @endcode
    class Scheduler
      : public elle::Printable
    {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      Scheduler();
      virtual
      ~Scheduler();

    /*------------------.
    | Current scheduler |
    `------------------*/
    public:
      /// Return a pointer to the current Scheduler.
      ///
      /// The current Scheduler is the one in charge of the Thread you are
      /// currently in.
      /// If you are not in a Thread, return a null pointer.
      ///
      /// @returns Pointer to current Scheduler.
      static
      Scheduler*
      scheduler();

    /*------.
    | Debug |
    `------*/
    public:
      /// Dump the state of the Scheduler in the standard error stream.
      ///
      /// Display all threads managed by the Scheduler, sorted by current
      /// state (see Thread::State).
      void
      dump_state();

    /*----.
    | Run |
    `----*/
    public:
      /// Start the Scheduler cycles and block until all Threads are
      /// terminated.
      void
      run();
      /// Perfom cycle of the Scheduler.
      bool
      step();
    private:
      virtual
      void
      _rethrow_exception(std::exception_ptr e) const;
      void _step(Thread* t);
      ELLE_ATTRIBUTE_R(bool, done);

    /*-------------------.
    | Threads management |
    `-------------------*/
    public:
      using Threads = boost::multi_index::multi_index_container<
      Thread*,
      boost::multi_index::indexed_by<
        boost::multi_index::hashed_unique<boost::multi_index::identity<Thread*>>,
        boost::multi_index::sequenced<>
        >
      >;
      /// Return a pointer to the current Thread.
      ///
      /// @pre Being called from a Thread managed by a scheduler.
      ///
      /// @returns Pointer the the current Thread.
      Thread*
      current() const;
      /// Mark for termination all running Threads and return the list of
      /// affected Threads.
      Threads
      terminate();
      /// Terminate all running Threads and wait until they exit.
      void
      terminate_now();
      /// Notify the Scheduler this is shall stop.
      ///
      /// See `_shallstop' boolean.
      void
      terminate_later();
    private:
      /// If set, the Scheduler will mark every running Thread for termination
      /// at the end of the next Thread::step.
      ELLE_ATTRIBUTE(bool, shallstop);

    private:
      void
      _freeze(Thread& thread);
      void
      _thread_register(Thread& thread);
      void
      _unfreeze(Thread& thread, std::string const& reason);
    private:
      /// Terminate the given Thread.
      ///
      /// @param thread Pointer to the Thread to terminate.
      /// @returns Whether the Thread is done.
      bool
      _terminate(Thread* thread);
      /// Terminate the given Thread and wait until it's done.
      ///
      /// If the given Thread is the current Thread and suicide is false, this
      /// is a noop.
      ///
      /// @param thread A pointer to the Thread to terminate.
      /// @param suicide Whether the Thread should suicide if trying to
      ///        terminate itself.
      void
      _terminate_now(Thread* thread,
                     bool suicide);
      ELLE_ATTRIBUTE(Thread*, current);
      ELLE_ATTRIBUTE(Threads, starting);
      ELLE_ATTRIBUTE(std::mutex, starting_mtx);
      ELLE_ATTRIBUTE(Threads, running);
      ELLE_ATTRIBUTE(Threads, frozen);

    /*-------------------------.
    | Thread Exception Handler |
    `-------------------------*/
    protected:
      std::exception_ptr _eptr;

    /*----------------.
    | Multithread API |
    `----------------*/
    public:
      /// Allow for using the Scheduler in a multi-threaded environment.
      ///
      /// @tparam R The return-type of the given function.
      /// @param name A descriptive name of Thread to be spawn.
      /// @param action The function to be run.
      /// @returns The result of invoking `action`.
      template <typename R>
      R
      mt_run(std::string const& name,
             std::function<R ()> const& action);

    /*----------.
    | Printable |
    `----------*/
    public:
      void
      print(std::ostream& s) const override;

    /*----------.
    | Shortcuts |
    `----------*/
    public:
      /// Run the given operation in the next cycle.
      ///
      /// @param name A descriptive name of the operation, for debugging.
      /// @param f The operation to run later.
      void
      run_later(std::string const& name,
                std::function<void ()> const& f);
      /// Run the given operation after a given delay.
      ///
      /// @param name A descriptive name of the operation, for debugging.
      /// @param f The operation to run later.
      /// @param delay The delay before running the operation.
      void
      call_later(std::string const& name,
                 std::function<void ()> const& f,
                 Duration delay);

    /*----------------.
    | Background jobs |
    `----------------*/
    public:
      /// Number of threads spawned to run background jobs.
      int
      background_pool_size() const;
    private:
      template <typename T>
      friend class BackgroundOperation;
      /// Run function in a system thread. Run the result back in the scheduler.
      ///
      /// It is guaranteed that a thread will immediately start executing the
      /// action, spawning a new one if needed. The system thread runs freely,
      /// all race condition issues apply, use at your own risks. The thread is
      /// joined upon destruction of the scheduler. The returned function is
      /// then run in the scheduler context.
      ///
      /// This pattern enables to run absolutely pure code in the system thread,
      /// removing the possibility of any race condition, and run the
      /// potentially non-pure epilogue in the non-parallel scheduler context.
      ///
      /// @param action The Action to run in a system thread.
      void
      _run_background(std::function<std::function<void ()> ()> action);
      ELLE_ATTRIBUTE(boost::asio::io_service, background_service);
      ELLE_ATTRIBUTE(std::unique_ptr<boost::asio::io_service::work>, background_service_work);
      ELLE_ATTRIBUTE(std::vector<std::thread>, background_pool);
      ELLE_ATTRIBUTE(int, background_pool_free);
      friend
      void
      background(std::function<void()> const& action);

    /*--------.
    | Signals |
    `--------*/
    public:
      /// Connect the given handler to a signal.
      ///
      /// @param signal  The macro constant identifying the signal.
      /// @param handler The operation to run if the signal is triggered.
      void
      signal_handle(int signal, std::function<void()> const& handler);
    private:
      ELLE_ATTRIBUTE(std::vector<std::unique_ptr<boost::asio::signal_set>>,
                     signal_handlers);

      /*-----.
      | Asio |
      `-----*/
    public:
      ELLE_ATTRIBUTE_RX(boost::asio::io_service, io_service);
      ELLE_ATTRIBUTE(std::unique_ptr<boost::asio::io_service::work>, io_service_work);

    /*--------.
    | Details |
    `--------*/
    public:
      /// Get the backend::Backend used to manage coroutines.
      ///
      /// @returns Non-const reference to the manager.
      backend::Backend&
      manager();
    private:
      friend class Thread;
      std::unique_ptr<backend::Backend> _manager;
      std::thread::id _running_thread;
    };

    /*---------------.
    | Free functions |
    `---------------*/

    /// The current scheduler.
    reactor::Scheduler&
    scheduler();
    /// Run an action in a system thread and yield until completion.
    ///
    /// @param action The action to run in background.
    void
    background(std::function<void()> const& action);
    /// Yield execution for this scheduler round.
    void
    yield();
    /// Yield for @a duration.
    void
    sleep(Duration duration);
    /// Yield forever.
    void
    sleep();
    /// Wait for @a waitable.
    bool
    wait(Waitable& waitable, DurationOpt timeout = {});
    /// Wait for @a waitables.
    bool
    wait(Waitables const& waitables, DurationOpt timeout = {});
    /// Wait for @a waitables.
    bool
    wait(std::vector<std::reference_wrapper<Waitable>> const& waitables,
         DurationOpt timeout = {});
    /// Wait until @a signal is emitted.
    void
    wait(boost::signals2::signal<void ()>& signal);

    /// A barrier waiting for a signal.
    class Waiter;

    /// A barrier opened when @a signal is triggered, validated either
    /// by a predicate, or by a comparison to a reference value.
    template <typename Prototype, typename ... Args>
    Waiter
    waiter(boost::signals2::signal<Prototype>& signal, Args&&... args);

    /// Wait for a signal, validated either by a predicate, or by a
    /// comparison to a reference value.
    template <typename Prototype, typename ... Args>
    void
    wait(boost::signals2::signal<Prototype>& signal, Args&&... args);

    /// Run the given operation in the next cycle.
    ///
    /// @param name A descriptive name of the operation, for debugging.
    /// @param f    Operation to run later.
    ///
    void
    run_later(std::string const& name, std::function<void ()> const& f);

    /// Run the given operation now, or in the next cycle.
    ///
    /// @param async  Whether to run later.
    /// @param name   A descriptive name of the operation, for debugging.
    /// @param f      Operation to run later.
    ///
    template <typename Fun>
    void
    run(bool later, std::string const& name, Fun f);
  }
}

#include <elle/reactor/scheduler.hxx>
