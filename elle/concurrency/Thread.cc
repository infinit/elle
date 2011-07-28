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
// updated       julien quintard   [thu jul 28 16:22:46 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Thread.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>
#include <elle/standalone/Log.hh>

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
	alert(_(), "an error occured in this thread");

      // process events.
      // XXX if (Application::Process() == StatusError)
      //alert("an error occured while processing events");
      // process ses propres events!

      release();
    }

  }
}
