//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/network/slot/Node.hh
//
// created       julien quintard   [fri nov 27 22:03:15 2009]
// updated       julien quintard   [wed mar 17 14:05:56 2010]
//

#ifndef ELLE_TEST_NETWORK_NODE_HH
#define ELLE_TEST_NETWORK_NODE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>
#include <elle/network/Network.hh>

#include <elle/test/network/slot/Table.hh>
#include <elle/test/network/slot/Manifest.hh>

namespace elle
{
  using namespace core;
  using namespace misc;
  using namespace network;

  namespace test
  {

//
// ---------- classes ---------------------------------------------------------
//

    class Node
    {
    public:
      //
      // constants
      //
      static const Natural32			Rate = 10000;

      //
      // methods
      //
      Status		Setup(const String&,
			      const String&,
			      const Port);

      Status		Run();

      //
      // attributes
      //
      String		host;
      Port		port;

      Slot		slot;

      String		name;
      Table		table;

      //
      // callbacks
      //
      Status		Handle(String&,
			       Table&);
      Status		Refresh();
    };

  }
}

#endif
