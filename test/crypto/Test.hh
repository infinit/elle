//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       GPL
//
// file          /home/mycure/infinit/elle/test/crypto/Test.hh
//
// created       julien quintard   [wed jan 28 11:28:16 2009]
// updated       julien quintard   [thu feb 19 23:48:13 2009]
//

#ifndef ELLE_TEST_ARCHIVE_TEST_HH
#define ELLE_TEST_ARCHIVE_TEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>
#include <elle/crypto/Cryptography.hh>

#include "Random.hh"

#include <iostream>

namespace elle
{
  using namespace core;
  using namespace misc;
  using namespace crypto;

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

