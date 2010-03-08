//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/test/network/slot/Manifest.hh
//
// created       julien quintard   [tue feb 23 21:40:13 2010]
// updated       julien quintard   [fri mar  5 15:32:31 2010]
//

#ifndef ELLE_TEST_NETWORK_SLOT_PROBE_HH
#define ELLE_TEST_NETWORK_SLOT_PROBE_HH

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

        TagProbe,

	Tags = TagProbe + 1
      };

  }
}

//
// ---------- definitions -----------------------------------------------------
//

Outward(::elle::test::TagProbe,
	Parameters(String));

#endif
