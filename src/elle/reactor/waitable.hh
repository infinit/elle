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
    class Waitable:
      public elle::Printable
    {
    /*---------.
    | Typedefs |
    `---------*/
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
      Waitable(std::string  name = std::string());
      /// Move a Waitable.
      Waitable(Waitable&& source);
    protected:
      /// Destroy a Waitable.
      ~Waitable();
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
      bool
      wait(DurationOpt timeout = DurationOpt());

    protected:
      /** Wake a thread when we are done, if we must be waited.
       *
       * If true is returned, the thread will block until we call its
       * wake method. Otherwise it will carry on, and its wake method
       * should not be called.
       *
       * The default implementation always returns true and registers
       * the thread for wake. By overriding this method, one can
       * create a waitable that does not always block by either
       * returning false, either calling the parent method and
       * returning true.
       *
       * @param thread The thread to wake up when we're done.
       * @return Whether the thread needs to wait for us.
       */
      virtual
      bool
      _wait(Thread* thread, Waker const& waker);
      /** Abort waking a thread when we are done.
       *
       * Forget about a thread previously successfully registered with
       * _wait. This happen in the case of timeouts, for
       * instance. Overriding this method can be useful to cancel an
       * operation if nobody waits on it anymore, for instance. The
       * parent method must still be called, to avoid trying to wake
       * up the thread erroneously when we're done.
       */
      virtual
      void
      _unwait(Thread* thread);
      /// Wake waiting threads.
      int
      _signal();
      virtual
      Thread*
      _signal_one();
      void
      _signal_one(Thread* thread);
      void
      _signal_one(Handler const& thread);
      /** Register an exception waiting thread should throw when woken.
       *
       * @param e The exception threads must throw.
       */
      template <typename Exception, typename... Args>
      void
      _raise(Args&&... args);
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
    /// Pretty print Waitables
    std::ostream&
    operator << (std::ostream& stream, Waitables const& waitables);
  }
}

#include <elle/reactor/waitable.hxx>
