//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/remote/Remote.hh
//
// created       julien quintard   [fri may 20 19:31:08 2011]
// updated       julien quintard   [sun aug 28 20:23:11 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_REMOTE_REMOTE_HH
#define HOLE_IMPLEMENTATIONS_REMOTE_REMOTE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <hole/Holeable.hh>

#include <hole/implementations/remote/Peer.hh>
#include <hole/implementations/remote/Client.hh>
#include <hole/implementations/remote/Server.hh>

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
	// enumerations
	//
	enum Role
	  {
	    RoleUnknown,
	    RoleServer,
	    RoleClient
	  };

	//
	// static attributes
	//
	static Remote*		Machine;

	//
	// constructors & destructors
	//
	Remote(const nucleus::Network&);
	~Remote();

	//
	// interfaces
	//

	// holeable
	elle::Status		Join();
	elle::Status		Leave();

	elle::Status		Put(const nucleus::Address&,
				    const nucleus::ImmutableBlock&);
	elle::Status		Put(const nucleus::Address&,
				    const nucleus::MutableBlock&);
	elle::Status		Get(const nucleus::Address&,
				    nucleus::ImmutableBlock&);
	elle::Status		Get(const nucleus::Address&,
				    const nucleus::Version&,
				    nucleus::MutableBlock&);
	elle::Status		Kill(const nucleus::Address&);

	//
	// interfaces
	//

	// dumpable
	elle::Status		Dump(const elle::Natural32 = 0) const;

	//
	// attributes
	//
	Role			role;

	union
	{
	  Server*		server;
	  Client*		client;

	  Peer*			peer;
	};
      };

    }
  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <hole/implementations/remote/Manifest.hh>
#include <hole/implementations/remote/Customer.hh>

#endif
