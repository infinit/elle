//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri nov 27 22:03:15 2009]
//

#ifndef ELLE_TEST_NETWORK_LOCAL_SERVER_HH
#define ELLE_TEST_NETWORK_LOCAL_SERVER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <elle/test/network/local/Manifest.hh>

#include <elle/idiom/Close.hh>
# include <list>
#include <elle/idiom/Open.hh>

namespace elle
{
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
      Status            Setup(const String&);
      Status            Run();

      //
      // callbacks
      //
      Status            Connection(LocalSocket*);

      //
      // attributes
      //
      String                    line;
      std::list<LocalSocket*>   sockets;
    };

  }
}

#endif
