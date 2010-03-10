//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Thread.cc
//
// created       julien quintard   [tue mar  9 18:52:40 2010]
// updated       julien quintard   [wed mar 10 13:31:01 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Thread.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method returns the current thread.
    ///
    Thread*		Thread::Domain()
    {
      return ((Thread*)::QThread::currentThread());
    }
  }
}
