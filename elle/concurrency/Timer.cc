//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Timer.cc
//
// created       julien quintard   [wed mar 17 12:11:11 2010]
// updated       julien quintard   [thu mar 25 13:27:19 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Timer.hh>
#include <elle/concurrency/Fiber.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method sets up the timer by recording the callback.
    ///
    Status		Timer::Create(const Mode		mode,
				      Callback<>&		callback)
    {
      enter();

      // set the mode attribute.
      this->mode = mode;

      // set the timer mode.
      if (this->mode == Timer::ModeSingle)
	this->timer.setSingleShot(true);

      // set the callback.
      this->callback = callback;

      // generate an event identifier related to this timer.
      if (this->event.Generate() == StatusError)
	escape("unable to generate the event");

      // connect the signal.
      if (this->connect(&this->timer, SIGNAL(timeout()),
			this, SLOT(Timeout())) == false)
	escape("unable to connect the timeout signal");

      leave();
    }

    ///
    /// this method starts the timer according the the mode and duration.
    ///
    Status		Timer::Start(Natural32			duration)
    {
      enter();

      // start the timer.
      this->timer.start(duration);

      leave();
    }

    ///
    /// this method stops the timer.
    ///
    Status		Timer::Stop()
    {
      enter();

      // stop the timer.
      this->timer.stop();

      leave();
    }

    ///
    /// this method restarts the timer according the the mode and duration.
    ///
    Status		Timer::Restart(Natural32		duration)
    {
      enter();

      // restart the timer.
      this->timer.start(duration);

      leave();
    }

//
// ---------- slots -----------------------------------------------------------
//

    ///
    /// this slot is triggered whenever the timer timeouts.
    ///
    void		Timer::Timeout()
    {
      Closure<>		closure(this->callback);

      enter();

      // awaken the fibers waiting for this event.
      if (Fiber::Awaken(this->event) == StatusError)
	alert("unable to awaken the fibers");

      // spawn a fiber by providing the callback's closure.
      if (Fiber::Spawn(closure) == StatusError)
	alert("an error occured while spawning the fiber");

      release();
    }

  }
}
