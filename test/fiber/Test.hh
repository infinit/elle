//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/test/fiber/Test.hh
//
// created       julien quintard   [wed jan 28 11:28:16 2009]
// updated       julien quintard   [fri apr  9 15:59:21 2010]
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

