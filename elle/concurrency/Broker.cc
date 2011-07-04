//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Broker.cc
//
// created       julien quintard   [sun may 29 14:29:01 2011]
// updated       julien quintard   [mon jul  4 11:53:38 2011]
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
    Broker::Broker(const Natural16				descriptor,
		   const Callback<
		     Parameters<const Natural16> >&		callback):
      descriptor(descriptor),
      callback(callback),
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

      // trigger the callback.
      if (this->callback.Trigger(this->descriptor) == StatusError)
	escape("an error occured while spawning the fiber");

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
      Callback< Parameters<> >	callback(&Broker::Trigger, this);
      Closure< Parameters<> >	closure(callback);

      enter();

      // spawn a fiber.
      if (Fiber::Spawn(closure) == StatusError)
	alert(_(), "unable to spawn a fiber");

      release();
    }

  }
}
