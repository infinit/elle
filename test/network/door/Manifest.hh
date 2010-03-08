//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/test/network/door/Manifest.hh
//
// created       julien quintard   [tue feb 23 21:40:13 2010]
// updated       julien quintard   [fri mar  5 15:32:05 2010]
//

#ifndef ELLE_TEST_NETWORK_DOOR_ECHO_HH
#define ELLE_TEST_NETWORK_DOOR_ECHO_HH

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

        TagChallenge,
        TagResponse,

	Tags = TagResponse + 1
      };

  }
}

//
// ---------- definitions -----------------------------------------------------
//

Outward(::elle::test::TagChallenge,
	Parameters(String));
Inward(::elle::test::TagResponse,
       Parameters(String));

#endif
