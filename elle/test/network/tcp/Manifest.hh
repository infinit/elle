//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue feb 23 21:40:13 2010]
//

#ifndef ELLE_TEST_NETWORK_TCP_MANIFEST_HH
#define ELLE_TEST_NETWORK_TCP_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Network.hh>

//
// ---------- tags ------------------------------------------------------------
//

namespace elle
{
  namespace test
  {

    enum Tag
      {
	TagNone = 0,

        TagChallenge = 1000,
        TagResponse,

	Tags = TagResponse + 1
      };

  }
}

//
// ---------- definitions -----------------------------------------------------
//

outward(elle::test::TagChallenge,
	parameters(elle::core::String&));
inward(elle::test::TagResponse,
       parameters(elle::core::String&));

#endif
