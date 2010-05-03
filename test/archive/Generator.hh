//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/test/archive/Generator.hh
//
// created       julien quintard   [wed jan 28 12:57:36 2009]
// updated       julien quintard   [sun may  2 21:54:43 2010]
//

#ifndef ELLE_TEST_ARCHIVE_GENERATOR_HH
#define ELLE_TEST_ARCHIVE_GENERATOR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include "Pack.hh"
#include "Test.hh"
#include "Random.hh"

namespace elle
{
  namespace test
  {

//
// ---------- classes ---------------------------------------------------------
//

    class Generator
    {
    public:
      //
      // constants
      //
      static const Integer64	MinimumBooleanSize;
      static const Integer64	MaximumBooleanSize;

      static const Integer64	MinimumCharacterSize;
      static const Integer64	MaximumCharacterSize;

      static const Integer64	MinimumRealSize;
      static const Integer64	MaximumRealSize;

      static const Integer64	MinimumInteger8Size;
      static const Integer64	MaximumInteger8Size;

      static const Integer64	MinimumInteger16Size;
      static const Integer64	MaximumInteger16Size;

      static const Integer64	MinimumInteger32Size;
      static const Integer64	MaximumInteger32Size;

      static const Integer64	MinimumInteger64Size;
      static const Integer64	MaximumInteger64Size;

      static const Integer64	MinimumNatural8Size;
      static const Integer64	MaximumNatural8Size;

      static const Integer64	MinimumNatural16Size;
      static const Integer64	MaximumNatural16Size;

      static const Integer64	MinimumNatural32Size;
      static const Integer64	MaximumNatural32Size;

      static const Integer64	MinimumNatural64Size;
      static const Integer64	MaximumNatural64Size;

      static const Integer64	MinimumLargeSize;
      static const Integer64	MaximumLargeSize;

      static const Integer64	MinimumStringLength;
      static const Integer64	MaximumStringLength;

      static const Integer64	MinimumRegionSize;
      static const Integer64	MaximumRegionSize;

      static const Integer64	MinimumArchiveChoice;
      static const Integer64	MaximumArchiveChoice;

      static const Integer64	MinimumArchiveSize;
      static const Integer64	MaximumArchiveSize;

      //
      // methods
      //
      static Status		Create(Boolean&);
      static Status		Create(Character&);
      static Status		Create(Real&);
      static Status		Create(Integer8&);
      static Status		Create(Integer16&);
      static Status		Create(Integer32&);
      static Status		Create(Integer64&);
      static Status		Create(Natural8&);
      static Status		Create(Natural16&);
      static Status		Create(Natural32&);
      static Status		Create(Natural64&);
      static Status		Create(Large&);
      static Status		Create(String&);
      static Status		Create(Region&);
      static Status		Create(Archive&);
    };

  }
}

#endif
