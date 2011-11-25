//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri nov 27 22:03:15 2009]
//

#ifndef ELLE_TEST_NETWORK_TCP_CLIENT_HH
#define ELLE_TEST_NETWORK_TCP_CLIENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <elle/test/network/tcp/Manifest.hh>

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

    class Client
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
      Status		Challenge(const String&);

      //
      // attributes
      //
      Locus		locus;
      TCPSocket		socket;
    };

  }
}

#endif
