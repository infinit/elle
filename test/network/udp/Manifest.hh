//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue feb 23 21:40:13 2010]
//

#ifndef ELLE_TEST_NETWORK_UDP_MANIFEST_HH
#define ELLE_TEST_NETWORK_UDP_MANIFEST_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Network.hh>

#include <elle/test/network/udp/Table.hh>

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
        parameters(elle::core::String&,
                   elle::test::Table&));

#endif
