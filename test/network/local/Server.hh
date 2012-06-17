#ifndef ELLE_TEST_NETWORK_LOCAL_SERVER_HH
# define ELLE_TEST_NETWORK_LOCAL_SERVER_HH

# include <elle/types.hh>
# include <elle/network/fwd.hh>

# include <elle/test/network/local/Manifest.hh>

# include <elle/idiom/Close.hh>
#  include <list>
# include <elle/idiom/Open.hh>

namespace elle
{
  namespace test
  {
    // XXX
    using namespace elle::network;

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
    };

  }
}

#endif
