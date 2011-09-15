//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed jan 28 11:28:16 2009]
//

#ifndef ELLE_TEST_PACKAGE_TEST_HH
#define ELLE_TEST_PACKAGE_TEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include "Pack.hh"
#include "Random.hh"
#include "Generator.hh"

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
      static const Natural32		MinimumPackSize;
      static const Natural32		MaximumPackSize;
    };

  }
}

#endif

