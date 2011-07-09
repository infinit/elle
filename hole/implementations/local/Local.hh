//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/local/Local.hh
//
// created       julien quintard   [tue apr 13 16:05:04 2010]
// updated       julien quintard   [wed jul  6 15:22:40 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_LOCAL_LOCAL_HH
#define HOLE_IMPLEMENTATIONS_LOCAL_LOCAL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <hole/Holeable.hh>

namespace hole
{
  namespace implementations
  {
    ///
    /// XXX
    ///
    namespace local
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// the local hole implementation stores data on the local node's
      /// operating system storage.
      ///
      class Local:
	public Holeable
      {
      public:
	//
	// constructors & destructors
	//
	Local(const nucleus::Network&);

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
      };

    }
  }
}

#endif
