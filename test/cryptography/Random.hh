//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/test/cryptography/Random.hh
//
// created       julien quintard   [thu jan 29 11:52:52 2009]
// updated       julien quintard   [thu oct 15 20:11:38 2009]
//

#ifndef ELLE_TEST_CRYPTOGRAPHY_RANDOM_HH
#define ELLE_TEST_CRYPTOGRAPHY_RANDOM_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

namespace elle
{
  using namespace core;
  using namespace miscellaneous;

  namespace test
  {

//
// ---------- classes ---------------------------------------------------------
//

    class Random
    {
    public:
      //
      // methods
      //
      static Integer64	Generate(Integer64		minimum,
				 Integer64		maximum)
      {
	Integer64	base = (Integer64)rand();

	if ((base >= minimum) && (base <= maximum))
	  return (base);

	return (base % ((maximum - minimum) + 1) + minimum);
      }
    };

  }
}

#endif
