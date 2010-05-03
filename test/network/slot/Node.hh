//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/test/network/slot/Node.hh
//
// created       julien quintard   [fri nov 27 22:03:15 2009]
// updated       julien quintard   [sun may  2 21:58:51 2010]
//

#ifndef ELLE_TEST_NETWORK_NODE_HH
#define ELLE_TEST_NETWORK_NODE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <elle/test/network/slot/Table.hh>
#include <elle/test/network/slot/Manifest.hh>

namespace elle
{
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
      Status		Handle(const String&,
			       const Table&);
      Status		Refresh();
    };

  }
}

#endif
