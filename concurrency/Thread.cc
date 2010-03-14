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
// updated       julien quintard   [sun mar 14 13:05:45 2010]
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
    Thread*		Thread::Retrieve()
    {
      return ((Thread*)::QThread::currentThread());
    }

    ///
    /// this method starts the thread.
    ///
    Status		Thread::Start()
    {
      enter();

      this->start();

      leave();
    }

    ///
    /// this method implements the run() method which will be called
    /// by the parent class.
    ///
    void		Thread::run()
    {
      enter();

      // call the Run method.
      if (this->Run() == StatusError)
	alert("an error occured in this thread");

      // process events.
      if (Event::Process() == StatusError)
	alert("an error occured while processing events");

      release();
    }

  }
}
