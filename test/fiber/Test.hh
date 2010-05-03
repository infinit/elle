//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/test/fiber/Test.hh
//
// created       julien quintard   [wed jan 28 11:28:16 2009]
// updated       julien quintard   [wed apr 28 21:39:46 2010]
//

#ifndef ELLE_TEST_FIBER_TEST_HH
#define ELLE_TEST_FIBER_TEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <elle/idiom/Close.hh>
# include <iostream>
#include <elle/idiom/Open.hh>

namespace elle
{
  namespace test
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this test is supposed to be scheduled as follows:
    ///
    /// F1 -> wait ResourceA -> awaken ResourceB
    /// F2 -> wait ResourceB -> awaken ResourceC
    /// F3 -> spawn F4 -> awaken ResourceA
    ///    -> wait ResourceC
    ///
    /// order: F1, F2, F3, F4, F3, F1, F2, F3
    ///
    class Test
    {
    public:
      //
      // constants
      //
      static Resource	ResourceA;
      static Resource	ResourceB;
      static Resource	ResourceC;

      //
      // attributes
      //
      static Timer	Timer1;
      static Timer	Timer2;
      static Timer	Timer3;
    };

  }
}

#endif

