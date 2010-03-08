//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/network/door/Server.hh
//
// created       julien quintard   [fri nov 27 22:03:15 2009]
// updated       julien quintard   [fri mar  5 23:14:39 2010]
//

#ifndef ELLE_TEST_NETWORK_SERVER_HH
#define ELLE_TEST_NETWORK_SERVER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <QObject>
#include <QThread>
#include <QTimer>

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>
#include <elle/network/Network.hh>

#include <elle/test/network/door/Manifest.hh>

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

    class Thread1:
      public ::QThread
    {
      Q_OBJECT;
    };

    class Server:
      public Thread1,
      public Callable
    {
    public:
      //
      // methods
      //
      void		run();

      //
      // callbacks
      //
      Status		Connection(Door*&);

      //
      // attributes
      //
      String		name;
      std::list<Door*>	doors;
    };

  }
}

#endif
