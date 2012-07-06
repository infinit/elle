#ifndef HOLE_IMPLEMENTATIONS_REMOTE_REMOTE_HH
# define HOLE_IMPLEMENTATIONS_REMOTE_REMOTE_HH

# include <elle/types.hh>

# include <hole/implementations/remote/fwd.hh>

namespace hole
{
  namespace implementations
  {
    ///
    /// this namespace contains the remote hole implementation.
    ///
    /// note that for this implementation, hole can be run alone so as to
    /// join a network at startup.
    ///
    /// if this network relies on a remote model, the implementation starts
    /// by trying to connect to the server. should this step fail, a server
    /// is spawn in order to wait for connections.
    ///
    /// this way a single implementation emulates both the client and server.
    ///
    /// the remote hole implementation stores data on a remote host's
    /// storage.
    ///
    namespace remote
    {

      ///
      /// this class controls the remote implementation.
      ///
      class Remote
      {
      public:
        //
        // static attributes
        //
        static Machine*         Computer;
      };

    }
  }
}

#endif
