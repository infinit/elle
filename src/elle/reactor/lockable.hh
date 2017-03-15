#pragma once

#include <elle/reactor/Waitable.hh>

namespace elle
{
  namespace reactor
  {
    /// Lockable is an abstract type derived from Waitable to provide a
    /// synchronization mechanism.
    ///
    /// Waiting on a unlocked Lockable will result on a noop. Threads waiting on
    /// a locked Lockable will be awaken when the Lockable is released.
    ///
    /// Class inheriting from Lockable must implement acquire and release.
    /// - acquire must try to 'lock' the Lockable and return whether it was
    ///   successful of not.
    /// - release must 'unlock' the Lockable and return whether the Lockable is
    ///   unlocked.
    class Lockable
      : public Waitable
    {
    public:
      /// Try to acquire the lock, without blocking.
      ///
      /// \returns Whether the acquisition was successful (Lockable wasn't
      ///          already lock).
      virtual
      bool
      acquire() = 0;
      /// Release the lock.
      virtual
      bool
      release() = 0;
    };

    /// Lock is designed to manage Lockable automatically via RAII.
    ///
    /// The calling Thread will wait until the Lock manages to acquire the
    /// Lockable. Lock destructor will release the Lockable.
    ///
    /// \code{.cc}
    ///
    /// void
    /// do_something(reactor::Lockable& lockable)
    /// {
    ///   {
    ///     auto lock = Lock{lockable}; // Wait until Lockable is acquired.
    ///     // We are sure the Lockable is acquired.
    ///     // ...
    ///   } // Destructor of the Lock will release the Lockable.
    /// }
    ///
    /// \endcode
    class Lock
    {
    public:
      /// Take a Lockable and wait until it acquires it.
      ///
      /// \param lockable The Lockable to acquire.
      Lock(Lockable& lockable);
      /// Release the Lockable.
      ~Lock();

    private:
      Lockable& _lockable;
    };
  }
}
