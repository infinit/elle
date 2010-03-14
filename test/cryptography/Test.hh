//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/cryptography/Test.hh
//
// created       julien quintard   [wed jan 28 11:28:16 2009]
// updated       julien quintard   [wed mar 10 21:08:53 2010]
//

#ifndef ELLE_TEST_CRYPTOGRAPHY_TEST_HH
#define ELLE_TEST_CRYPTOGRAPHY_TEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include "Random.hh"

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
      // enumerations
      //
      enum Type
	{
	  TypeKeys,
	  TypeEncryption,
	  TypeSignature,
	  TypeCipher
	};

      //
      // constants
      //
      static const Natural32		MinimumTestsNumber;
      static const Natural32		MaximumTestsNumber;

      static const Natural32		MinimumRegionSize;
      static const Natural32		MaximumRegionSize;

      static const Natural32		MinimumKeyLength;
      static const Natural32		MaximumKeyLength;

      //
      // methods
      //
      static Status	Generate(Region&);
      static Status	Main();
    };

  }
}

#endif

