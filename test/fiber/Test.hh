//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/fiber/Test.hh
//
// created       julien quintard   [wed jan 28 11:28:16 2009]
// updated       julien quintard   [mon mar 29 19:18:11 2010]
//

#ifndef ELLE_TEST_CRYPTOGRAPHY_TEST_HH
#define ELLE_TEST_CRYPTOGRAPHY_TEST_HH

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
      // methods
      //
      static Status	Generate(Region&);
    };

  }
}

#endif

