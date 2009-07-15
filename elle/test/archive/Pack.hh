//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       GPL
//
// file          /home/mycure/infinit/elle/test/archive/Pack.hh
//
// created       julien quintard   [tue jan 27 16:45:41 2009]
// updated       julien quintard   [fri feb  6 21:03:01 2009]
//

#ifndef ELLE_TEST_ARCHIVE_PACK_HH
#define ELLE_TEST_ARCHIVE_PACK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/archive/Archive.hh>

#include "Referee.hh"

namespace elle
{
  using namespace core;
  using namespace misc;
  using namespace archive;

  namespace test
  {

//
// ---------- classes ---------------------------------------------------------
//

    class Pack
    {
    public:
      //
      // methods
      //
      static Status	Create(Archive&,
			       const Natural32,
			       const Boolean);
      static Status	Verify(Archive&);
    };

  }
}

#endif
