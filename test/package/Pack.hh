//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/test/package/Pack.hh
//
// created       julien quintard   [tue jan 27 16:45:41 2009]
// updated       julien quintard   [fri jun  3 15:08:05 2011]
//

#ifndef ELLE_TEST_PACKAGE_PACK_HH
#define ELLE_TEST_PACKAGE_PACK_HH

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
