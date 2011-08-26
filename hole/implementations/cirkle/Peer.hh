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
// updated       julien quintard   [fri aug 26 18:42:07 2011]
//
 
#ifndef HOLE_IMPLEMENTATIONS_CIRKLE_PEER_HH
#define HOLE_IMPLEMENTATIONS_CIRKLE_PEER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

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

	elle::Status		Propagate(Neighbour*);

	//
	// interfaces
	//

	//
	// callbacks
	//
	elle::Status		Connection(elle::Gate*&);

	elle::Status		Update(const Cluster&);

	//
	// interfaces
	//

	// dumpable
	elle::Status		Dump(const elle::Natural32 = 0) const;

	//
	// attributes
	//
	Neighbourhood		neighbourhood;
	RoutingTable		table;
      };

    }
  }
}

#endif
