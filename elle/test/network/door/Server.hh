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
// updated       julien quintard   [sun feb  7 01:41:07 2010]
//

#ifndef ELLE_TEST_NETWORK_SERVER_HH
#define ELLE_TEST_NETWORK_SERVER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>
#include <elle/network/Network.hh>

#include <elle/test/network/door/Echo.hh>

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

    class Server:
      public ::QObject,
      public Callable
    {
      Q_OBJECT;

    public:
      //
      // methods
      //
      Status		Start(const String&);

      //
      // callbacks
      //
      Status		Connection(Door*&);
    };

  }
}

#endif
