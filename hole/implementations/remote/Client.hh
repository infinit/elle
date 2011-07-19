//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/remote/Client.hh
//
// created       julien quintard   [thu may 26 10:21:46 2011]
// updated       julien quintard   [mon jul 18 20:20:34 2011]
//
 
#ifndef HOLE_IMPLEMENTATIONS_REMOTE_CLIENT_HH
#define HOLE_IMPLEMENTATIONS_REMOTE_CLIENT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <hole/implementations/remote/Node.hh>

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
      /// XXX
      ///
      class Client:
	public Node
      {
      public:
	//
	// constructors & destructors
	//
	Client(const nucleus::Network&,
	       const elle::Address&);
	~Client();

	//
	// interfaces
	//

	// node
	elle::Status	Initialize();
	elle::Status	Clean();

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
	// callbacks
	//
	elle::Status	Challenge(elle::Cipher&);

	elle::Status	Error(const elle::String&);

	//
	// interfaces
	//

	// dumpable
	elle::Status	Dump(const elle::Natural32 = 0) const;

	//
	// attributes
	//
	elle::Gate*	gate;
      };

    }
  }
}

#endif
