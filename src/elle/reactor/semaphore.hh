#pragma once

#include <elle/attribute.hh>
#include <elle/reactor/lockable.hh>

namespace elle
{
  namespace reactor
  {
    /// A Semaphore can be acquired simultaneously by a given number of Threads,
    /// forcing following Threads trying to acquire it to wait until it's
    /// released by one of the current acquirer.
    ///
    /// Semaphore, like std::semaphore, helps protecting resources from
    /// 'concurrent' access, which also happen in a co-routine environment.
    class Semaphore
      : public Lockable
    {
    public:
      /// Create a Semaphore.
      ///
      /// \param count The number of simultaneous acquirers.
      Semaphore(int count = 0);
      /// Try to acquire the Semaphore.
      bool
      acquire() override;
      /// Release the Semaphore.
      bool
      release() override;

    protected:
      /// Specialization of Lockable::_wait.
      ///
      /// Wait only and only if count is inferior or equal to zero.
      ///
      /// \param thread A pointer to the Thread that may wait.
      /// \param waker The Waker to invoke.
      /// \returns Whether the Thread waited.
      bool
      _wait(Thread* thread, Waker const& waker) override;

    private:
      /// Number of potential acquirers.
      ELLE_ATTRIBUTE_R(int, count);
    };
  }
}
