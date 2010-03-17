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
// updated       julien quintard   [wed mar 17 12:49:12 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Timer.hh>

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
      callback(NULL)
    {
    }

    ///
    /// the destructor.
    ///
    Timer::~Timer()
    {
      // release the callback.
      if (this->callback != NULL)
	delete this->callback;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method sets up the timer by recording the callback.
    ///
    Status		Timer::Create(const Mode		mode,
				      Callback&			callback)
    {
      enter();

      // clone the given callback.
      if (callback.Clone((Entity*&)this->callback) == StatusError)
	escape("unable to clone the callback");

      // set the mode attribute.
      this->mode = mode;

      // set the timer mode.
      if (this->mode == Timer::ModeSingle)
	this->timer.setSingleShot(true);

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
      // trigger the callback.
      this->callback->Trigger();
    }

  }
}
