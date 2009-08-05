//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/benchmark/crypto/Benchmark.hh
//
// created       julien quintard   [wed jan 28 11:28:16 2009]
// updated       julien quintard   [wed aug  5 15:48:57 2009]
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

    class Benchmark
    {
    public:
      //
      // constants
      //
      static const Natural32		AsymmetricKeyLength;
      static const Natural32		SymmetricKeyLength;

      static const Natural32		BlockSize;

      //
      // methods
      //
      static Status	Main();
    };

  }
}

#endif

