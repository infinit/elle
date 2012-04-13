//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed mar 17 11:22:38 2010]
//

#ifndef ELLE_TEST_NETWORK_UDP_NEIGHBOUR_HH
#define ELLE_TEST_NETWORK_UDP_NEIGHBOUR_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

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
      public Object
    {
    public:
      //
      // constants
      //
      static const Natural32            Timeout = 20000;

      //
      // attributes
      //
      Node*             node;

      Timer             timer;

      Locus             locus;
      String            name;

      //
      // methods
      //
      Status            Create(Node*,
                               const Locus&,
                               const String&);
      Status            Update(const String&);
      Status            Refresh();

      //
      // interfaces
      //

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      // archivable
      Status            Serialize(Archive&) const;
      Status            Extract(Archive&);

      //
      // callbacks
      //
      Status            Discard();
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <elle/test/network/udp/Node.hh>

#endif
