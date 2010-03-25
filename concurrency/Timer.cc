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
// updated       julien quintard   [thu mar 25 01:04:30 2010]
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
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor.
    ///
    Timer::Timer():
      entrance(NULL)
    {
    }

    ///
    /// the destructor.
    ///
    Timer::~Timer()
    {
      // release the entrance.
      if (this->entrance != NULL)
	delete this->entrance;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method sets up the timer by recording the entrance.
    ///
    Status		Timer::Create(const Mode		mode,
				      Entrance&			entrance)
    {
      enter();

      // set the mode attribute.
      this->mode = mode;

      // set the timer mode.
      if (this->mode == Timer::ModeSingle)
	this->timer.setSingleShot(true);

      // clone the given entrance.
      if (entrance.Clone((Entity*&)this->entrance) == StatusError)
	escape("unable to clone the entrance");

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
      enter();

      // awaken the fibers waiting for this event.
      if (Fiber::Awaken(this->event) == StatusError)
	alert("unable to awaken the fibers");

      // trigger the entrance in a fresh fiber.
      if (Fiber::Spawn(this->entrance) == StatusError)
	alert("an error occured while spawning the fiber");

      release();
    }

  }
}
