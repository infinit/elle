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
// updated       julien quintard   [sun sep  4 16:00:10 2011]
//

#ifndef ELLE_TEST_CRYPTOGRAPHY_RANDOM_HH
#define ELLE_TEST_CRYPTOGRAPHY_RANDOM_HH

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

    ///
    /// this class provides methods for generating random numbers.
    ///
    class Random
    {
    public:
      //
      // methods
      //

      ///
      /// this method generates a random number.
      ///
      static Integer64	Generate(Integer64			minimum,
				 Integer64			maximum)
      {
	Integer64	base = (Integer64)rand();

	if ((base >= minimum) && (base <= maximum))
	  return (base);

	return (base % ((maximum - minimum) + 1) + minimum);
      }

      ///
      /// this method generates a random region.
      ///
      static Status	Generate(Region&			region)
      {
	static const Natural32		MinimumRegionSize = 1;
	static const Natural32		MaximumRegionSize = 12345;

	Natural32	size = Random::Generate(MinimumRegionSize,
						MaximumRegionSize);
	Byte*		buffer;
	Natural32	i;

	enter(slab(buffer, ::free));

	// allocate a buffer.
	if ((buffer = (Byte*)::malloc(size)) == NULL)
	  escape("unable to allocate memory");

	// randomize the buffer contents.
	for (i = 0; i < size; i++)
	  *(buffer + i) =
	    Random::Generate(elle::core::Type<Character>::Minimum,
			     elle::core::Type<Character>::Maximum);

	// assign the buffer to the region.
	if (region.Acquire(buffer, size) == StatusError)
	  escape("unable to assign the buffer to the region");

	// since included in the region.
	waive(buffer);

	leave();
      }
    };

  }
}

#endif
