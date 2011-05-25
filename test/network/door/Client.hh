//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/test/network/door/Client.hh
//
// created       julien quintard   [fri nov 27 22:03:15 2009]
// updated       julien quintard   [wed may 25 16:09:03 2011]
//

#ifndef ELLE_TEST_NETWORK_DOOR_CLIENT_HH
#define ELLE_TEST_NETWORK_DOOR_CLIENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <elle/test/network/door/Manifest.hh>

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
      String		line;
      Door		door;
    };

  }
}

#endif
