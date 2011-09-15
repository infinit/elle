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

#include <elle/concurrency/Broker.hh>

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
    Broker::Broker(const Natural16				descriptor):
      descriptor(descriptor),
      notifier(descriptor, ::QSocketNotifier::Read)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method activates the broker.
    ///
    Status		Broker::Activate()
    {
      enter();

      if (this->connect(&this->notifier, SIGNAL(activated(int)),
			this, SLOT(_trigger())) == false)
	escape("unable to connect the signal");

      leave();
    }

    ///
    /// this method deactivates the broker.
    ///
    Status		Broker::Deactivate()
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
    Status		Broker::Trigger()
    {
      enter();

      // emit the signal.
      if (this->signal.ready.Emit(this->descriptor) == StatusError)
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
    void		Broker::_trigger()
    {
      Closure< Status,
	       Parameters<> >	closure(Callback<>::Infer(&Broker::Trigger,
							  this));

      enter();

      //
      // the following part should not be necessary but it turns out
      // that QT triggers this event even though there is nothing to
      // be read on the socket.
      //
      // \todo XXX check in future versions to be sure that this bug
      //           diseppear: launching Infinit and trying CTRL^C does
      //           not work without this code.
      {
	struct timeval	timeout = { 0, 0 };
	::fd_set	set;

	FD_ZERO(&set);

	FD_SET(this->descriptor, &set);

	if (::select(FD_SETSIZE, &set, NULL, NULL, &timeout) == 0)
	  return;
      }

      // spawn a fiber.
      if (Fiber::Spawn(closure) == StatusError)
	yield(_(), "unable to spawn a fiber");

      release();
    }

  }
}
