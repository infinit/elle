//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Accord.cc
//
// created       julien quintard   [sun mar  7 19:07:03 2010]
// updated       julien quintard   [sun jun 19 22:33:54 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Accord.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method locks in reading or writing.
    ///
    Status		Accord::Lock(const Mode			mode,
				     const Natural32		timeout)
    {
      enter();

      // lock depending on the mode.
      switch (mode)
	{
	case ModeWrite:
	  {
	    // lock.
	    if (timeout > 0)
	      {
		if (this->accord.tryLockForWrite(timeout) == false)
		  escape("unable to lock the resources in time");
	      }
	    else
	      this->accord.lockForWrite();

	    break;
	  }
	case ModeRead:
	  {
	    // lock.
	    if (timeout > 0)
	      {
		if (this->accord.tryLockForRead(timeout) == false)
		  escape("unable to lock the resources in time");
	      }
	    else
	      this->accord.lockForRead();

	    break;
	  }
	}

      leave();
    }

    ///
    /// this method releases the lock.
    ///
    Status		Accord::Unlock()
    {
      enter();

      // release the resources.
      this->accord.unlock();

      leave();
    }

  }
}
