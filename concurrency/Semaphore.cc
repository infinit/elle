//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Semaphore.cc
//
// created       julien quintard   [sun mar  7 19:07:03 2010]
// updated       julien quintard   [thu mar 25 00:31:22 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Semaphore.hh>

///
/// these includes are placed here in order to prevent pre-processing
/// conflicts.
///
#include <elle/misc/Report.hh>
#include <elle/misc/Maid.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method acquires N resources or waits for their availability.
    ///
    Status		Semaphore::Acquire(const Natural32	n,
					   const Natural32	timeout)
    {
      enter();

      // acquire n resources.
      if (timeout > 0)
	{
	  if (this->semaphore.tryAcquire(n, timeout) == false)
	    escape("unable to acquire the resources in time");
	}
      else
	this->semaphore.acquire(n);

      leave();
    }

    ///
    /// this method releases N resources.
    ///
    Status		Semaphore::Release(const Natural32	n)
    {
      ///
      /// this is done because release() conflict with the Elle release
      /// macro function :(
      ///
      static void	(::QSemaphore::*f)(int) = &::QSemaphore::release;

      enter();

      // release the resources.
      (this->semaphore.*f)(n);

      leave();
    }

  }
}
