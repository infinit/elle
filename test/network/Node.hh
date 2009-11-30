//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/network/Node.hh
//
// created       julien quintard   [fri nov 27 22:03:15 2009]
// updated       julien quintard   [mon nov 30 14:54:07 2009]
//

#ifndef ELLE_TEST_NETWORK_NODE_HH
#define ELLE_TEST_NETWORK_NODE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>
#include <elle/network/Network.hh>

#include <elle/test/network/Probe.hh>

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

      Location		location;
      String		name;
      ::QTimer		timer;

    private slots:
      //
      // slots
      //
      void		Discard();
    };

    class Node:
      public ::QObject
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
			      const Natural16);

      Status		Add(const Location&,
			    const String&);
      Status		Remove(const Location&);
      Status		Update(const Location&,
			       const String&);
      Status		Locate(const Location&,
			       Iterator&);

      //
      // attributes
      //
      String		name;
      ::QTimer*		timer;
      Container		container;
      Socket		socket;

      //
      // callbacks
      //
      void		Handle(const Location&,
			       const Probe&);

    private slots:
      //
      // slots
      //
      void		Refresh();
    };

  }
}

#endif
