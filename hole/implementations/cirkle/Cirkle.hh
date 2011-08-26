//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/cirkle/Cirkle.hh
//
// created       julien quintard   [fri aug 12 13:49:49 2011]
// updated       julien quintard   [thu aug 25 19:47:03 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_CIRKLE_CIRKLE_HH
#define HOLE_IMPLEMENTATIONS_CIRKLE_CIRKLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <hole/Holeable.hh>
#include <hole/implementations/cirkle/Peer.hh>

namespace hole
{
  namespace implementations
  {
    ///
    /// XXX
    ///
    namespace cirkle
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      ///
      class Cirkle:
	public Holeable
      {
      public:
	//
	// constants
	//
	static const elle::Natural16			Port;

	//
	// static attributes
	//
	static Peer	Machine;

	//
	// static methods
	//
	static elle::Status	Show(const elle::Natural32);

	//
	// constructors & destructors
	//
	Cirkle(const nucleus::Network&);

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
      };

    }
  }
}

//
// ---------- includes --------------------------------------------------------
//

// XXX

#endif
