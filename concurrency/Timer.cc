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
// updated       julien quintard   [sun sep  4 15:43:32 2011]
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
      // stop and delete the timer, if present.
      if (this->timer != NULL)
	{
	  this->timer->stop();
	  this->timer->deleteLater();
	}
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method sets up the timer by recording the callback.
    ///
    Status		Timer::Create(const Mode		mode)
    {
      enter();

      // set the mode attribute.
      this->mode = mode;

      // allocate the timer.
      this->timer = new ::QTimer;

      // set the timer mode.
      if (this->mode == Timer::ModeSingle)
	this->timer->setSingleShot(true);

      // connect the signal.
      if (this->connect(this->timer, SIGNAL(timeout()),
			this, SLOT(_timeout())) == false)
	escape("unable to connect the timeout signal");

      leave();
    }

    ///
    /// this method starts the timer according the the mode and duration.
    ///
    Status		Timer::Start(const Natural32		duration)
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
    Status		Timer::Restart(const Natural32		duration)
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

      // emit the signal.
      if (this->signal.timeout.Emit() == StatusError)
	escape("unable to emit the signal");

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the timer.
    ///
    Status		Timer::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Timer]" << std::endl;

      // dump the mode.
      std::cout << alignment << Dumpable::Shift << "[Mode] "
		<< (const Natural8)this->mode << std::endl;

      // dump the interval.
      std::cout << alignment << Dumpable::Shift << "[Interval] "
		<< (const Natural32)this->timer->interval() << std::endl;

      // dump the signal.
      if (this->signal.timeout.Dump(margin + 2) == StatusError)
	escape("unable to dump the signal");

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
      Closure< Status,
	       Parameters<> >	closure(Callback<>::Infer(&Timer::Timeout,
							  this));

      enter();

      // trigger the callback in a new fiber.
      if (Fiber::Spawn(closure) == StatusError)
	yield(_(), "unable to spawn a fiber");

      release();
    }

  }
}
