#ifndef ELLE_TEST_NETWORK_LOCAL_CLIENT_HH
# define ELLE_TEST_NETWORK_LOCAL_CLIENT_HH

# include <elle/types.hh>

# include <elle/network/LocalSocket.hh>
# include <elle/test/network/local/Manifest.hh>

# include <elle/idiom/Close.hh>
#  include <list>
# include <elle/idiom/Open.hh>

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
      Status            Setup(const String&);
      Status            Run();

      //
      // callbacks
      //
      Status            Challenge(const String&);

      //
      // attributes
      //
      String            line;
      LocalSocket       socket;
    };

  }
}

#endif
