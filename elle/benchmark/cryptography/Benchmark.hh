//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed jan 28 11:28:16 2009]
//

#ifndef ELLE_BENCHMARK_ARCHIVE_TEST_HH
#define ELLE_BENCHMARK_ARCHIVE_TEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include "Time.hh"

#include <iostream>

namespace elle
{
  namespace benchmark
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class contains the benchmark metadata.
    ///
    class Benchmark
    {
    public:
      //
      // constants
      //
      static const Natural32		AsymmetricKeyLength;
      static const Natural32		SymmetricKeyLength;

      static const Natural32		BlockSize;
    };

  }
}

#endif

