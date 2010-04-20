//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infi...aries/elle/test/network/door/Manifest.hh
//
// created       julien quintard   [tue feb 23 21:40:13 2010]
// updated       julien quintard   [sun apr 18 20:05:52 2010]
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
	parameters(const elle::core::String));
inward(elle::test::TagResponse,
       parameters(const elle::core::String));

#endif
