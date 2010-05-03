//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Mutex.cc
//
// created       julien quintard   [sun mar  7 17:25:49 2010]
// updated       julien quintard   [sun may  2 17:21:44 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Mutex.hh>
#include <elle/concurrency/Fiber.hh>

#include <elle/standalone/Report.hh>
#include <elle/standalone/Maid.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Mutex::Mutex():
      state(Mutex::StateUnlocked)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method locks the mutex.
    ///
    Status		Mutex::Lock(const Natural32		timeout)
    {
      enter();

      // lock the mutex.
      if (timeout > 0)
	{
	  if (this->mutex.tryLock(timeout) == false)
	    escape("unable to lock the mutex in time");
	}
      else
	this->mutex.lock();

      // then, lock the state variable if possible or block the fiber
      // waiting for this resource.
      while (this->state == Mutex::StateLocked)
	{
	  // wait for the resource.
	  if (Fiber::Wait(this) == StatusError)
	    escape("an error occured while waiting for the mutex resource");

	}

      // at this point, the fiber acquired the lock, so let's change
      // the state.
      this->state = Mutex::StateLocked;

      leave();
    }

    ///
    /// this method unlocks the mutex.
    ///
    Status		Mutex::Unlock()
    {
      enter();

      // unlock the mutex.
      this->mutex.unlock();

      // unlock the state.
      this->state = Mutex::StateUnlocked;

      leave();
    }

  }
}
