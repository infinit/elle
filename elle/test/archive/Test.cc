//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/archive/Test.cc
//
// created       julien quintard   [wed jan 28 11:22:24 2009]
// updated       julien quintard   [thu aug  6 11:42:37 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include "Test.hh"

namespace elle
{
  using namespace misc;
  using namespace archive;

  namespace test
  {

//
// ---------- definitions -----------------------------------------------------
//

    const Natural32		Test::MinimumPackSize = 1;
    const Natural32		Test::MaximumPackSize = 987654;

//
// ---------- methods ---------------------------------------------------------
//

    Status		Test::Main()
    {
      Natural32		size;
      Archive		archive;
      Archive		ar;
      Archive		a;

      // initialise the random generator.
      ::srand(::getpid()); 

      // compute the archive's size.
      size = Random::Generate(Test::MinimumPackSize,
			      Test::MaximumPackSize);

      // prepare the archive.
      if (archive.Create() == StatusError)
	escape("unable to prepare the serialization archive");

      // generate the archive's contents.
      if (Pack::Create(archive,
		       size,
		       true) == StatusError)
	escape("unable to create a pack");

      // test the assignment.
      ar = archive;

      // test the comparison.
      if (archive != ar)
	escape("the two archives should be detected as identical");

      // detach the memory.
      ar.Detach();

      // prepare the archive to be extracted.
      if (a.Prepare(ar) == StatusError)
	escape("unable to prepare the extraction archive");

      // verify the archive.
      if (Pack::Verify(a) == StatusError)
	escape("an error has been detected while verifying the archive");

      leave();
    }

  }
}

//
// ---------- main ------------------------------------------------------------
//

int			main()
{
  if (elle::test::Test::Main() == elle::misc::StatusError)
    {
      std::cerr << elle::misc::report;

      return (1);
    }

  std::cout << "[success]" << std::endl;

  return (0);
}
