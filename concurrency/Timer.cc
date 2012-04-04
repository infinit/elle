//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed mar 17 12:11:11 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Program.hh>
#include <elle/concurrency/Timer.hh>
#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/idiom/Close.hh>
#include <reactor/thread.hh>
#include <elle/idiom/Open.hh>

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
          this->timer = NULL;
        }
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method sets up the timer by recording the callback.
    ///
    Status              Timer::Create(const Mode                mode)
    {
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

      return StatusOk;
    }

    ///
    /// this method starts the timer according the the mode and duration.
    ///
    Status              Timer::Start(const Natural32            duration)
    {
      // start the timer.
      this->timer->start(duration);

      return StatusOk;
    }

    ///
    /// this method stops the timer.
    ///
    Status              Timer::Stop()
    {
      // stop the timer.
      this->timer->stop();

      return StatusOk;
    }

    ///
    /// this method restarts the timer according the the mode and duration.
    ///
    Status              Timer::Restart(const Natural32          duration)
    {
      // restart the timer.
      this->timer->start(duration);

      return StatusOk;
    }

//
// ---------- callbacks -------------------------------------------------------
//

    ///
    /// this callback is triggered in a new fiber so that it can
    /// wait for events or resources.
    ///
    Status              Timer::Timeout()
    {
      // Q_EMIT the signal.
      if (this->signal.timeout.Emit() == StatusError)
        escape("unable to Q_EMIT the signal");

      return StatusOk;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the timer.
    ///
    Status              Timer::Dump(const Natural32             margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Timer]" << std::endl;

      // dump the mode.
      std::cout << alignment << Dumpable::Shift << "[Mode] "
                << this->mode << std::endl;

      // dump the interval.
      std::cout << alignment << Dumpable::Shift << "[Interval] "
                << this->timer->interval() << std::endl;

      // dump the signal.
      if (this->signal.timeout.Dump(margin + 2) == StatusError)
        escape("unable to dump the signal");

      return StatusOk;
    }

//
// ---------- slots -----------------------------------------------------------
//

    ///
    /// this slot is triggered whenever the timer timeouts.
    ///
    void                Timer::_timeout()
    {
      new reactor::Thread(scheduler(), "Timer timeout",
                          boost::bind(&Timer::Timeout, this), true);
    }

  }
}
