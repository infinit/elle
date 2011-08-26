//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/remote/Peer.hh
//
// created       julien quintard   [thu may 26 09:46:18 2011]
// updated       julien quintard   [thu aug 25 11:44:03 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_REMOTE_PEER_HH
#define HOLE_IMPLEMENTATIONS_REMOTE_PEER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

namespace hole
{
  namespace implementations
  {
    namespace remote
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// this class represents a peer being either a client or a server.
      ///
      class Peer:
	public elle::Entity
      {
      public:
	//
	// constructors & destructors
	//
	Peer(const nucleus::Network&);

	//
	// interface
	//
	virtual elle::Status	Initialize(const elle::Point&) = 0;
	virtual elle::Status	Clean() = 0;

	virtual elle::Status	Put(const nucleus::Address&,
				    const nucleus::ImmutableBlock&) = 0;
	virtual elle::Status	Put(const nucleus::Address&,
				    const nucleus::MutableBlock&) = 0;
	virtual elle::Status	Get(const nucleus::Address&,
				    nucleus::ImmutableBlock&) = 0;
	virtual elle::Status	Get(const nucleus::Address&,
				    const nucleus::Version&,
				    nucleus::MutableBlock&) = 0;
	virtual elle::Status	Kill(const nucleus::Address&) = 0;

	//
	// interfaces
	//

	// dumpable
	elle::Status		Dump(const elle::Natural32 = 0) const;

	//
	// attributes
	//
	nucleus::Network	network;
      };

    }
  }
}

#endif
