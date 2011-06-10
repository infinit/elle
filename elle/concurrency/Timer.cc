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
// updated       julien quintard   [tue jun  7 06:06:41 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Timer.hh>
#include <elle/concurrency/Fiber.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

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
    Timer::Timer():
      timer(NULL)
    {
    }

    ///
    /// destructor.
    ///
    Timer::~Timer()
    {
      // delete the timer, if present.
      if (this->timer != NULL)
	this->timer->deleteLater();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method sets up the timer by recording the callback.
    ///
    Status		Timer::Create(const Mode		mode,
				      const
				        Callback<
					  Parameters<> >&	callback)
    {
      enter();

      // set the mode attribute.
      this->mode = mode;

      // allocate the timer.
      this->timer = new ::QTimer;

      // set the timer mode.
      if (this->mode == Timer::ModeSingle)
	this->timer->setSingleShot(true);

      // set the callback.
      this->callback = callback;

      // connect the signal.
      if (this->connect(this->timer, SIGNAL(timeout()),
			this, SLOT(_timeout())) == false)
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
      this->timer->start(duration);

      leave();
    }

    ///
    /// this method stops the timer.
    ///
    Status		Timer::Stop()
    {
      enter();

      // stop the timer.
      this->timer->stop();

      leave();
    }

    ///
    /// this method restarts the timer according the the mode and duration.
    ///
    Status		Timer::Restart(Natural32		duration)
    {
      enter();

      // restart the timer.
      this->timer->start(duration);

      leave();
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback is triggered in a new fiber so that it can
    /// wait for events or resources.
    ///
    Status		Timer::Timeout()
    {
      enter();

      // trigger the callback.
      if (this->callback.Trigger() == StatusError)
	escape("an error occured while spawning the fiber");

      leave();
    }

//
// ---------- slots -----------------------------------------------------------
//

    ///
    /// this slot is triggered whenever the timer timeouts.
    ///
    void		Timer::_timeout()
    {
      Callback< Parameters<> >	callback(&Timer::Timeout, this);
      Closure< Parameters<> >	closure(callback);

      enter();

      // trigger the callback in a new fiber.
      if (Fiber::Spawn(closure) == StatusError)
	alert("unable to spawn a fiber");

      release();
    }

  }
}
