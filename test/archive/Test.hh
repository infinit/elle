//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/archive/Test.hh
//
// created       julien quintard   [wed jan 28 11:28:16 2009]
// updated       julien quintard   [sun mar  7 22:08:34 2010]
//

#ifndef ELLE_TEST_ARCHIVE_TEST_HH
#define ELLE_TEST_ARCHIVE_TEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include "Pack.hh"
#include "Random.hh"
#include "Generator.hh"

#include <iostream>

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

      //
      // methods
      //
      static Status	Main();
    };

  }
}

#endif

