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
// updated       julien quintard   [tue feb 23 01:50:42 2010]
//

#ifndef ELLE_TEST_NETWORK_NODE_HH
#define ELLE_TEST_NETWORK_NODE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>
#include <elle/network/Network.hh>

#include <elle/test/network/slot/Tag.hh>
#include <elle/test/network/slot/Probe.hh>

#include <QObject>
#include <QTimer>

#include <list>

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

    class Node;

    class		Neighbour:
      public ::QObject
    {
      Q_OBJECT;

    public:
      //
      // attributes
      //
      Node*		node;

      Address		address;
      String		name;
      ::QTimer		timer;

    private slots:
      //
      // slots
      //
      void		Discard();
    };

    class Node:
      public ::QObject,
      public Callable
    {
      Q_OBJECT;

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
      Status		Start(const String&,
			      const String&,
			      const Port);

      Status		Add(const Address&,
			    const String&);
      Status		Remove(const Address&);
      Status		Update(const Address&,
			       const String&);
      Status		Locate(const Address&,
			       Iterator&);

      //
      // attributes
      //
      String		name;
      ::QTimer*		timer;
      Container		container;
      Slot		slot;

      //
      // callbacks
      //
      Status		Handle(Environment&,
			       String&);

    private slots:
      //
      // slots
      //
      void		Refresh();
    };

  }
}

#endif
