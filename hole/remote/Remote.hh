//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/remote/Remote.hh
//
// created       julien quintard   [fri may 20 19:31:08 2011]
// updated       julien quintard   [thu may 26 15:02:39 2011]
//

#ifndef HOLE_LOCAL_REMOTE_HH
#define HOLE_LOCAL_REMOTE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <hole/Holeable.hh>

#include <hole/remote/Node.hh>

namespace hole
{
  ///
  /// this namespace contains the remote hole implementation.
  ///
  /// note that for this implementation, hole can be run alone so as to
  /// join a network at startup.
  ///
  /// if this network relies on a remote model, the implementation starts
  /// by trying to connect to the host. should this step fail, a server
  /// is spawn in order to wait for connections.
  ///
  /// this way a single implementation emulates both the client and server.
  ///
  namespace remote
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// the remote hole implementation stores data on a remote host's
    /// storage.
    ///
    class Remote:
      public Holeable
    {
    public:
      //
      // constructors & destructors
      //
      Remote(const nucleus::Network&);
      ~Remote();

      //
      // methods
      //
      elle::Status	Host(const elle::Address&);

      //
      // interfaces
      //

      // holeable
      elle::Status	Join();
      elle::Status	Leave();

      elle::Status	Put(const nucleus::Address&,
			    const nucleus::ImmutableBlock&);
      elle::Status	Put(const nucleus::Address&,
			    const nucleus::MutableBlock&);
      elle::Status	Get(const nucleus::Address&,
			    nucleus::ImmutableBlock&);
      elle::Status	Get(const nucleus::Address&,
			    const nucleus::Version&,
			    nucleus::MutableBlock&);
      elle::Status	Kill(const nucleus::Address&);

      //
      // attributes
      //
      elle::Address	host;

      Node*		node;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <hole/remote/Client.hh>
#include <hole/remote/Manifest.hh>
#include <hole/remote/Server.hh>

#endif
