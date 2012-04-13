//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sat nov 26 12:01:19 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Mutex.hh>
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
    Mutex::Zone::Zone(Mutex&                                    mutex):
      mutex(mutex),
      section(L(C(&Mutex::Lock, &this->mutex)),
              U(C(&Mutex::Unlock, &this->mutex)))
    {
    }

    ///
    /// this method locks the mutex.
    ///
    Void                Mutex::Zone::Lock()
    {
      // call the section's Enter() method.
      this->section.Enter();
    }

    ///
    /// this method unlocks the mutex.
    ///
    Void                Mutex::Zone::Unlock()
    {
      // call the section's Leave() method.
      this->section.Leave();
    }

    ///
    /// this method dumps the mutex.
    ///
    Status              Mutex::Zone::Dump(const Natural32       margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Zone]" << std::endl;

      // dump the mutex.
      if (this->mutex.Dump(margin + 2) == Status::Error)
        escape("unable to dump the mutex");

      // dump the section.
      if (this->section.Dump(margin + 2) == Status::Error)
        escape("unable to dump the section");

      return Status::Ok;
    }

//
// ---------- mutex ----------------------------------------------------------
//

    ///
    /// default constructor.
    ///
    Mutex::Mutex():
      locked(false)
    {
    }

    ///
    /// this method locks the mutex, if possible, or blocks the fiber
    /// until the mutex gets unlocked.
    ///
    Void                Mutex::Lock()
    {
      // has the lock been acquired, wait for it to get released.
      while (this->locked == true)
        {
          //
          // in this case, the current fiber must be blocked until
          // the mutex gets released.
          //

          // wait for the mutex.
          if (Fiber::Wait(*this) == Status::Error)
            yield(_(), "an error occured while waiting on the resource");
        }

      // first, set the mutex as being locked so that another writer
      // does not acquire it.
      this->locked = true;
    }

    ///
    /// this method unlocks the mutex.
    ///
    Void                Mutex::Unlock()
    {
      // reset the lock as being available;
      this->locked = false;

      // and finally, awaken the fibers potentially blocked on the
      // mutex.
      if (Fiber::Awaken(*this) == Status::Error)
        yield(_(), "unable to awaken the fibers");
    }

    ///
    /// this method returns true if the given access would be authorised
    /// without blocking.
    ///
    Status              Mutex::Try()
    {
      // has the lock been acquired, return false.
      if (this->locked == true)
        return Status::False;

      return Status::True;
    }

    ///
    /// this macro-function call generates the object.
    ///
    embed(Mutex, _());

    ///
    /// this method dumps the mutex.
    ///
    Status              Mutex::Dump(const Natural32             margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Mutex]" << std::endl;

      // dump the locked boolean.
      std::cout << alignment << Dumpable::Shift
                << "[Locked] " << this->locked << std::endl;

      return Status::Ok;
    }

  }
}
