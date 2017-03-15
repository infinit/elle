#pragma once

#include <elle/reactor/signals.hh>
#include <elle/reactor/Waitable.hh>

namespace elle
{
  namespace reactor
  {
    /// A multi lock checkpoint threads can wait upon.
    ///
    /// Threads waiting on a MultiLockBarrier will sleep until the barrier is
    /// opened. If the MultiLockBarrier is already opened, they will carry on
    /// without stopping.
    ///
    /// The concept is similar to a barrier, except multiple locks can be
    /// applied to the MultiLockBarrier (instead of a single boolean).
    /// By default, the MultiLockBarrier is opened and different threads can
    /// acquire a Lock via the lock() method.
    /// Each thread is responsible for the Lock lifetime.
    /// When all the Locks have been destroyed, the MultiLockBarrier signals
    /// those waiting on it.
    class MultiLockBarrier
      : public Waitable
    {
    /*------.
    | Types |
    `------*/
    public:
      /// Our own type.
      using Self = MultiLockBarrier;
      /// The parent type.
      using Super = Waitable;
    /*-------------.
    | Construction |
    `-------------*/
    public:
      /// Create a opened MultiLockBarrier with the given name.
      /// @param name The MultiLockBarrier name, for pretty-printing purpose.
      MultiLockBarrier(const std::string& name = std::string());
      virtual
      ~MultiLockBarrier();

    /*---------.
    | Openness |
    `---------*/
    private:
      /// A Lock for MultiLockBarrier.
      ///
      /// Increment the managed MultiLockBarrier lock count on construction, and
      /// decrease it on destruction.
      class Lock
      {
        Lock(MultiLockBarrier& bite);
      public:
        ~Lock();
        Lock(Lock&&) = default;
        Lock(Lock const&);
      private:
        ELLE_ATTRIBUTE(MultiLockBarrier&, barrier);
        friend MultiLockBarrier;
      };
    public:
      /// Whether there are no locks.
      operator bool() const;
    private:
      /// Number of locks on the barrier.
      ELLE_ATTRIBUTE_R(uint32_t, locks);

    public:
      /// Whether there are no locks.
      bool
      opened() const;

      // Obtain a lock.
      Lock
      lock();

    private:
      /// Add one lock to the barrier.
      void
      _lock();
      /// Remove a lock from the barrier.
      void
      _unlock();

    /*--------.
    | Waiting |
    `--------*/
    protected:
      /// Stop the thread if and only if this is closed.
      bool
      _wait(Thread* thread, Waker const& waker) override;

    /*------.
    | Hooks |
    `------*/
    public:
      ELLE_ATTRIBUTE_RX(boost::signals2::signal<void (uint32_t)>, changed);
    };
  }
}
