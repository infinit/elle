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
// updated       julien quintard   [thu apr  8 15:11:01 2010]
//

#ifndef ELLE_TEST_NETWORK_SERVER_HH
#define ELLE_TEST_NETWORK_SERVER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/miscellaneous/Miscellaneous.hh>
#include <elle/network/Network.hh>
#include <elle/concurrency/Concurrency.hh>

#include <elle/test/network/door/Manifest.hh>

#include <elle/idiom/Close.hh>
# include <list>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace miscellaneous;
  using namespace network;

  namespace test
  {

//
// ---------- classes ---------------------------------------------------------
//

    class Server
    {
    public:
      //
      // methods
      //
      Status		Setup(const String&);
      Status		Run();

      //
      // callbacks
      //
      Status		Connection(Door*&);

      //
      // attributes
      //
      String		line;
      std::list<Door*>	doors;
    };

  }
}

#endif
