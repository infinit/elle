//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sat nov 26 12:31:09 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Semaphore.hh>
#include <elle/concurrency/Fiber.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- zone ------------------------------------------------------------
//

    ///
    /// default constructor.
    ///
    Semaphore::Zone::Zone(Semaphore&                            semaphore,
                          const Natural32                       n):
      semaphore(semaphore),
      n(n),

      section(A(C(&Semaphore::Acquire, &this->semaphore), this->n),
              R(C(&Semaphore::Release, &this->semaphore), this->n))
    {
    }

    ///
    /// this method locks the semaphore according to the zone's resources.
    ///
    Void                Semaphore::Zone::Acquire()
    {
      // call the section's Enter() method.
      this->section.Enter();
    }

    ///
    /// this method unlocks the semaphore according to the zone's resources.
    ///
    Void                Semaphore::Zone::Release()
    {
      // call the section's Leave() method.
      this->section.Leave();
    }

    ///
    /// this method dumps the semaphore.
    ///
    Status              Semaphore::Zone::Dump(const Natural32   margin) const
    {
      String            alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Zone]" << std::endl;

      // dump the semaphore.
      if (this->semaphore.Dump(margin + 2) == StatusError)
        escape("unable to dump the semaphore");

      // dump the section.
      if (this->section.Dump(margin + 2) == StatusError)
        escape("unable to dump the section");

      leave();
    }

//
// ---------- semaphore -------------------------------------------------------
//

    ///
    /// default constructor.
    ///
    Semaphore::Semaphore(const Natural32                        n):
      acquired(0),
      available(n)
    {
    }

    ///
    /// this method acquires some resources from the semaphore, if possible,
    /// or blocks the fiber until the semaphore has enough available
    /// resources.
    ///
    Void                Semaphore::Acquire(const Natural32      n)
    {
      enter();

      // are there enough resources to be acquired...
      while (this->available < n)
        {
          //
          // in this case, the current fiber must be blocked until
          // the semaphore gets enough resources released.
          //

          // wait for the semaphore.
          if (Fiber::Wait(this) == StatusError)
            yield(_(), "an error occured while waiting on the resource");
        }

      // increase/decrease the number of acquired/available resources.
      this->acquired += n;
      this->available -= n;

      release();
    }

    ///
    /// this method releases some of the semaphore's resources.
    ///
    Void                Semaphore::Release(const Natural32      n)
    {
      enter();

      // decrease/increase the number of acquired/available resources.
      this->acquired -= n;
      this->available += n;

      // and finally, awaken the fibers potentially blocked on the
      // semaphore.
      if (Fiber::Awaken(this) == StatusError)
        yield(_(), "unable to awaken the fibers");

      release();
    }

    ///
    /// this method returns true if the given access would be authorised
    /// without blocking.
    ///
    Status              Semaphore::Try(const Natural32          n)
    {
      enter();

      // are there enough resources to be acquired...
      if (this->available < n)
        false();

      true();
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Semaphore, _());

    ///
    /// this method dumps the semaphore.
    ///
    Status              Semaphore::Dump(const Natural32         margin) const
    {
      String            alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Semaphore]" << std::endl;

      // dump the number of acquired resources.
      std::cout << alignment << Dumpable::Shift
                << "[Acquired] " << std::dec << this->acquired << std::endl;

      // dump the number of available resources.
      std::cout << alignment << Dumpable::Shift
                << "[Available] " << std::dec << this->available << std::endl;

      leave();
    }

  }
}
