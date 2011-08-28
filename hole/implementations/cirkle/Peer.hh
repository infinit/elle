//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/cirkle/Peer.hh
//
// created       julien quintard   [thu may 26 10:21:46 2011]
// updated       julien quintard   [sun aug 28 14:04:47 2011]
//
 
#ifndef HOLE_IMPLEMENTATIONS_CIRKLE_PEER_HH
#define HOLE_IMPLEMENTATIONS_CIRKLE_PEER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>

#include <hole/implementations/cirkle/Neighbourhood.hh>
#include <hole/implementations/cirkle/RoutingTable.hh>
#include <hole/implementations/cirkle/Cluster.hh>
#include <hole/implementations/cirkle/Neighbour.hh>

namespace hole
{
  namespace implementations
  {
    namespace cirkle
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      class Peer:
	public elle::Entity
      {
      public:
	//
	// enumerations
	//
	enum State
	  {
	    StateUnauthenticated,
	    StateAuthenticated
	  };

	//
	// constructors & destructors
	//
	Peer();

	//
	// methods
	//
	elle::Status		Initialize(const elle::Natural16,
					   const elle::Point&);
	elle::Status		Clean();

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

	//
	// callbacks
	//
	elle::Status		Connection(elle::Gate*&);
	elle::Status		Challenge();
	elle::Status		Authenticate(const lune::Passport&);
	elle::Status		Authenticated();
	elle::Status		Listen();
	elle::Status		Port(const elle::Port&);
	elle::Status		Request();
	elle::Status		Gossip(const Cluster&);

	//
	// interfaces
	//

	// dumpable
	elle::Status		Dump(const elle::Natural32 = 0) const;

	//
	// attributes
	//
	State			state;

	Neighbourhood		neighbourhood;
	RoutingTable		table;
      };

    }
  }
}

#endif
