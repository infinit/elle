//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed aug  5 15:25:54 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include "Time.hh"

namespace elle
{
  namespace benchmark
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method starts the timer.
    ///
    Status              Time::Start()
    {
      enter();

      ::gettimeofday(&this->start, NULL);

      leave();
    }

    ///
    /// this method stops the timer.
    ///
    Status              Time::Stop()
    {
      enter();

      ::gettimeofday(&this->stop, NULL);

      leave();
    }

    ///
    /// this method prints the elapsed time.
    ///
    Status              Time::Print()
    {
      Natural32         useconds;

      enter();

      useconds =
        ((this->stop.tv_sec * 1000000.0) + this->stop.tv_usec) -
        ((this->start.tv_sec * 1000000.0) + this->start.tv_usec);

      Real              mseconds;

      mseconds = (Real)useconds / 1000.0;

      std::cout << mseconds << " ms";

      leave();
    }

  }
}
