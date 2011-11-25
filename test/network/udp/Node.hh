//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri nov 27 22:03:15 2009]
//

#ifndef ELLE_TEST_NETWORK_UDP_NODE_HH
#define ELLE_TEST_NETWORK_UDP_NODE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <elle/test/network/udp/Table.hh>
#include <elle/test/network/udp/Manifest.hh>

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

      UDPSocket		socket;

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
