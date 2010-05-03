//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/test/archive/Pack.hh
//
// created       julien quintard   [tue jan 27 16:45:41 2009]
// updated       julien quintard   [sun may  2 21:54:31 2010]
//

#ifndef ELLE_TEST_ARCHIVE_PACK_HH
#define ELLE_TEST_ARCHIVE_PACK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include "Referee.hh"

namespace elle
{
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
