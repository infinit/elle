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
      ;

      ::gettimeofday(&this->start, NULL);

      return elle::StatusOk;
    }

    ///
    /// this method stops the timer.
    ///
    Status              Time::Stop()
    {
      ;

      ::gettimeofday(&this->stop, NULL);

      return elle::StatusOk;
    }

    ///
    /// this method prints the elapsed time.
    ///
    Status              Time::Print()
    {
      Natural32         useconds;

      ;

      useconds =
        ((this->stop.tv_sec * 1000000.0) + this->stop.tv_usec) -
        ((this->start.tv_sec * 1000000.0) + this->start.tv_usec);

      Real              mseconds;

      mseconds = (Real)useconds / 1000.0;

      std::cout << mseconds << " ms";

      return elle::StatusOk;
    }

  }
}
