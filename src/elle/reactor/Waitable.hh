#pragma once

#include <set>

#include <boost/function.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/noncopyable.hpp>

#include <elle/Exception.hh>
#include <elle/Printable.hh>
#include <elle/reactor/duration.hh>
#include <elle/reactor/fwd.hh>
#include <elle/reactor/signals.hh>

namespace elle
{
  namespace reactor
  {
    /// Waitable is a class Threads can wait for.
    ///
    /// In the Scheduler asynchroneous non-preemptive environment, Waitables
    /// offer a way for Threads to ensure they will stay frozen until a specific
    /// condition is fulfilled.
    ///
    /// Waitable::signal* are methods to schedule Waiters (Threads that are
    /// waiting) awakening.
    ///
    /// \code{.cc}
    ///
    /// auto w = Waitable();
    /// auto t1 = Thread(
    ///   scheduler, "one",
    ///   [w]
    ///   {
    ///      std::cout << "1";
    ///      w.wait();
    ///      std::cout << "2";
    ///   });
    /// auto t2 = Thread(
    ///   scheduler, "two",
    ///   [&w]
    ///   {
    ///      std::cout << "A";
    ///      Scheduler::yield();
    ///      std::cout << "B";
    ///      Scheduler::yield();
    ///      std::cout << "C";
    ///      w.signal();
    ///      std::cout << "D";
    ///      Scheduler::yield();
    ///      std::cout << "E";
    ///   });
    /// // Result: 1ABCD2E
    /// \endcode
    class Waitable:
      public elle::Printable
    {
    /*------.
    | Types |
    `------*/
    public:
      /// Self
      using Self = Waitable;
      /// Wake callback.
      using Waker = std::function<void (Thread*)>;
      /// Waited thread and its wake callback.
      using Handler = std::pair<Thread*, Waker>;
      /// Collection of threads waiting this
      using Waiters = boost::multi_index_container<
        Handler,
        boost::multi_index::indexed_by<
          boost::multi_index::sequenced<>,
          boost::multi_index::hashed_unique<
            boost::multi_index::member<Handler, Thread*, &Handler::first>>>
        >;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      /// Construct a Waitable.
      ///
      /// \param name A descriptive name of the Waitable, for debugging purpose.
      Waitable(std::string name = std::string());
      /// Move a Waitable.
      Waitable(Waitable&& source);
    protected:
      /// Destroy a Waitable.
      ~Waitable();
      /// Ensure the Waiter has no Threads waiting for it before destroying it.
      void
      _assert_destructible();

    /*-------.
    | Status |
    `-------*/
    public:
      ELLE_ATTRIBUTE_R(std::string, name);
      ELLE_ATTRIBUTE_R(Waiters, waiters);

    /*--------.
    | Waiting |
    `--------*/
    public:
      /// Make the current Thread wait for this Waitable.
      ///
      /// If no \timeout is specified, the Thread will wait until the Waitable
      /// is done. Otherwise, the Thread will be awoken automatically if the
      /// Waitable didn't complete within duration set by \timeout.
      ///
      /// \param timeout The duration to wait before giving up.
      /// \returns Whether the Thread was awoken because the Waitable is done
      ///         (not because wait timed out).
      bool
      wait(DurationOpt timeout = DurationOpt());

    protected:
      /// Wake a Thread when we are done, if we must be waited.
      ///
      /// If true is returned, the thread will block until we call its
      /// wake method. Otherwise it will carry on, and its wake method
      /// should not be called.
      ///
      /// The default implementation always returns true and registers
      /// the thread for wake. By overriding this method, one can
      /// create a waitable that does not always block by either
      /// returning false, either calling the parent method and
      /// returning true.
      ///
      /// \param thread The Thread to wake up when we're done.
      /// \param waker The Waker to invoke.
      /// \returns Whether the thread needs to wait for us.
      virtual
      bool
      _wait(Thread* thread, Waker const& waker);
      /// Abort waking a Thread when we are done.
      ///
      /// Forget about a thread previously successfully registered with
      /// _wait. This happen in the case of timeouts, for
      /// instance. Overriding this method can be useful to cancel an
      /// operation if nobody waits on it anymore, for instance. The
      /// parent method must still be called, to avoid trying to wake
      /// up the thread erroneously when we're done.
      ///
      /// \param thread A pointer to Thread that should be awaken by this
      ///               Waitable.
      virtual
      void
      _unwait(Thread* thread);
      /// Wake waiting threads.
      ///
      /// \returns The number of Threads awaken.
      int
      _signal();
      /// Signal a Thread to wake up.
      ///
      /// \returns A pointer to the Thread awoken.
      virtual
      Thread*
      _signal_one();
      /// Signal \a specific Thread to wake up.
      ///
      /// If the Thread is not waiting for us, this is an noop.
      ///
      /// \param thread The Thread to wake up.
      void
      _signal_one(Thread* thread);
      /// Signal a specific Handler.
      ///
      /// Same as _signal_one(Thread). If the Handler has a Waker function, use
      /// it.
      ///
      /// \param thread The Handler to signal.
      void
      _signal_one(Handler const& thread);
      ///  Register an exception waiting thread should throw when woken.
      ///
      /// \tparam Exception The type of the exception to raise.
      /// \tparam Args Types of the exception's arguments.
      /// \param args The arguments of exception to throw.
      template <typename Exception, typename... Args>
      void
      _raise(Args&&... args);
      /// Register an exception waiting Thread should throw when woken.
      ///
      /// \param e The exception Thread must throw.
      void
      _raise(std::exception_ptr e);

    private:
      /// Let friends register/unregister themselves.
      friend class Thread;
      friend class OrWaitable;
      /// Exception woken thread must throw.
      ELLE_ATTRIBUTE_R(std::exception_ptr, exception);

    /*-------.
    | Events |
    `-------*/
    public:
      /// Signal triggered when the waitable wakes its waiting threads.
      ELLE_ATTRIBUTE_RX(boost::signals2::signal<void ()>, on_signaled);

    /*----------.
    | Printable |
    `----------*/
    public:
      void
      print(std::ostream& stream) const override;
    };

    /// Add a Waitable into Waitables.
    Waitables& operator << (Waitables& waitables, Waitable& s);
    /// Add a Waitable into Waitables.
    Waitables& operator << (Waitables& waitables, Waitable* s);
    /// Pretty print Waitables.
    std::ostream&
    operator << (std::ostream& stream, Waitables const& waitables);
  }
}

#include <elle/reactor/Waitable.hxx>
