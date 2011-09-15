//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [thu jan 29 11:52:52 2009]
//

#ifndef ELLE_TEST_PACKAGE_RANDOM_HH
#define ELLE_TEST_PACKAGE_RANDOM_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace elle
{
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
	Integer64	base = static_cast<Integer64>(rand());

	if ((base >= minimum) && (base <= maximum))
	  return (base);

	return (base % ((maximum - minimum) + 1) + minimum);
      }
    };

  }
}

#endif
