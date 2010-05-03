//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/test/network/slot/Neighbour.hh
//
// created       julien quintard   [wed mar 17 11:22:38 2010]
// updated       julien quintard   [sun may  2 22:58:28 2010]
//

#ifndef ELLE_TEST_NETWORK_SLOT_NEIGHBOUR_HH
#define ELLE_TEST_NETWORK_SLOT_NEIGHBOUR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace elle
{
  namespace test
  {

//
// ---------- forward declarations --------------------------------------------
//

    class Node;

//
// ---------- classes ---------------------------------------------------------
//

    class Neighbour:
      public Object<>
    {
    public:
      //
      // constants
      //
      static const Natural32		Timeout = 20000;

      //
      // attributes
      //
      Node*		node;

      Timer		timer;

      Address		address;
      String		name;

      //
      // methods
      //
      Status		Create(Node*,
			       const Address&,
			       const String&);
      Status		Update(const String&);
      Status		Refresh();

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // callbacks
      //
      Status		Discard();
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <elle/test/network/slot/Node.hh>

#endif
