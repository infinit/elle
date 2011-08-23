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
// updated       julien quintard   [fri aug 12 16:31:46 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_CIRKLE_CIRKLE_HH
#define HOLE_IMPLEMENTATIONS_CIRKLE_CIRKLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <hole/implementations/cirkle/Host.hh>

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
	// constructors & destructors
	//
	Cirkle(const nucleus::Network&);
	~Cirkle();

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
	// interfaces
	//

	// dumpable
	elle::Status	Dump(const elle::Natural32 = 0) const;

	//
	// attributes
	//
	//Node*		node;
      };

    }
  }
}

//
// ---------- includes --------------------------------------------------------
//

// XXX

#endif
