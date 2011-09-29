//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sun may 29 14:29:01 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/WinEventBroker.hh>

#include <elle/concurrency/Callback.hh>
#include <elle/concurrency/Closure.hh>
#include <elle/concurrency/Fiber.hh>

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
    WinEventBroker::WinEventBroker(HANDLE       hEvent):
      notifier(hEvent)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method activates the broker.
    ///
    Status		WinEventBroker::Activate()
    {
      enter();

      if (this->connect(&this->notifier, SIGNAL(activated(HANDLE)),
			this, SLOT(_trigger())) == false)
	escape("unable to connect the signal");

      leave();
    }

    ///
    /// this method deactivates the broker.
    ///
    Status		WinEventBroker::Deactivate()
    {
      enter();

      if (this->disconnect(&this->notifier, SIGNAL(activated(int)),
			   this, SLOT(_trigger())) == false)
	escape("unable to disconnect from the signal");

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback is triggered in a new fiber so that it can
    /// wait for events or resources.
    ///
    Status		WinEventBroker::Trigger()
    {
      enter();

      // emit the signal.
      if (this->signal.ready.Emit(this->notifier.handle()) == StatusError)
	escape("unable to emit the signal");

      leave();
    }

//
// ---------- slots -----------------------------------------------------------
//

    ///
    /// this slot is launched whenever data is present on the file descriptor.
    ///
    /// the callback is therefored triggered.
    ///
    void		WinEventBroker::_trigger()
    {
      Closure< Status,
	       Parameters<> >	closure(Callback<>::Infer(&WinEventBroker::Trigger,
							  this));

      enter();

      // spawn a fiber.
      if (Fiber::Spawn(closure) == StatusError)
	yield(_(), "unable to spawn a fiber");

      release();
    }

  }
}
