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
// updated       julien quintard   [sun mar  7 19:14:34 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Mutex.hh>

namespace elle
{
  namespace concurrency
  {

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

      leave();
    }

  }
}
