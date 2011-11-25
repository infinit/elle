//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed mar 17 13:07:53 2010]
//

#ifndef ELLE_TEST_NETWORK_UDP_TABLE_HH
#define ELLE_TEST_NETWORK_UDP_TABLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <elle/idiom/Close.hh>
# include <list>
#include <elle/idiom/Open.hh>

namespace elle
{
  namespace test
  {

//
// ---------- forward declarations --------------------------------------------
//

    class Node;
    class Neighbour;

//
// ---------- classes ---------------------------------------------------------
//

    class Table:
      public Object
    {
    public:
      //
      // types
      //
      typedef std::list<Neighbour*>		Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // methods
      //
      Status		Create(Node*);

      Status		Update(const Locus&,
			       const String&);
      Status		Refresh(const Locus&);
      Status		Remove(const Locus&);
      Status		Locate(const Locus&,
			       Iterator&);

      Status		Merge(const Table&);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      Node*		node;

      Timer		timer;

      Container		container;

      //
      // callbacks
      //
      Status		Renew();
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <elle/test/network/udp/Node.hh>
#include <elle/test/network/udp/Neighbour.hh>

#endif
