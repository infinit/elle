//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/network/door/Client.hh
//
// created       julien quintard   [fri nov 27 22:03:15 2009]
// updated       julien quintard   [fri mar  5 21:24:30 2010]
//

#ifndef ELLE_TEST_NETWORK_CLIENT_HH
#define ELLE_TEST_NETWORK_CLIENT_HH

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

    class Client:
      public ::QThread,
      public Callable
    {
      Q_OBJECT;

    public:
      //
      // methods
      //
      void		run();

      //
      // callbacks
      //
      Status		Challenge(String&);

      //
      // attributes
      //
      String		name;
      Door		door;
    };

  }
}

#endif
