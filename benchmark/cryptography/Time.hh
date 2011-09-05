//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/benchmark/cryptography/Time.hh
//
// created       julien quintard   [wed aug  5 15:23:38 2009]
// updated       julien quintard   [sat sep  3 19:55:45 2011]
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
