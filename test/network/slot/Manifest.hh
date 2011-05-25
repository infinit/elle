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
// updated       julien quintard   [wed may 25 16:09:30 2011]
//

#ifndef ELLE_TEST_NETWORK_SLOT_MANIFEST_HH
#define ELLE_TEST_NETWORK_SLOT_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Network.hh>

#include <elle/test/network/slot/Table.hh>

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

        TagProbe = 1000,

	Tags = TagProbe + 1
      };

  }
}

//
// ---------- definitions -----------------------------------------------------
//

outward(elle::test::TagProbe,
	parameters(const elle::core::String,
		   const elle::test::Table));

#endif
