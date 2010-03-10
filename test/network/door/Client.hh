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
// updated       julien quintard   [wed mar 10 17:14:15 2010]
//

#ifndef ELLE_TEST_NETWORK_CLIENT_HH
#define ELLE_TEST_NETWORK_CLIENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>
#include <elle/network/Network.hh>
#include <elle/concurrency/Concurrency.hh>

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

    class Client
      //public Thread
    {
    public:
      //
      // methods
      //
      Status		Run();

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
