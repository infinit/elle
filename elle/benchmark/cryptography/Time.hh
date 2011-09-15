//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed aug  5 15:23:38 2009]
//

#ifndef ELLE_BENCHMARK_CRYPTOGRAPHY_TIME_HH
#define ELLE_BENCHMARK_CRYPTOGRAPHY_TIME_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <sys/time.h>

namespace elle
{
  namespace benchmark
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// the specific class is used for recording the elapsed time.
    ///
    class Time
    {
    public:
      //
      // methods
      //
      Status			Start();
      Status			Stop();
      Status			Print();

      //
      // attributes
      //
      struct ::timeval		start;
      struct ::timeval		stop;
    };

  }
}

#endif
