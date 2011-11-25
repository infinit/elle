//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri nov 27 22:03:15 2009]
//

#ifndef ELLE_TEST_NETWORK_TCP_SERVER_HH
#define ELLE_TEST_NETWORK_TCP_SERVER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <elle/test/network/tcp/Manifest.hh>

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
      // constructors & destructors
      //
      Server();
      ~Server();

      //
      // methods
      //
      Status		Setup(const String&);
      Status		Run();

      //
      // callbacks
      //
      Status		Connection(TCPSocket*);

      //
      // attributes
      //
      Locus		locus;
      TCPSocket*	socket;
    };

  }
}

#endif
