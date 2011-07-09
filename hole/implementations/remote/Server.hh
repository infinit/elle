//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/remote/Server.hh
//
// created       julien quintard   [wed may 25 19:20:52 2011]
// updated       julien quintard   [fri jul  8 10:55:46 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_REMOTE_SERVER_HH
#define HOLE_IMPLEMENTATIONS_REMOTE_SERVER_HH

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
      class Server:
	public Node
      {
      public:
	//
	// constructors & destructors
	//
	Server(const nucleus::Network&,
	       const elle::Address&);
	~Server();

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
	elle::Status	Push(const nucleus::Address&,
			     const nucleus::Derivable<nucleus::Block>&);
	elle::Status	Pull(const nucleus::Address&,
			     const nucleus::Version&);
	elle::Status	Wipe(const nucleus::Address&);

	elle::Status	Connection(elle::Gate*&);
	elle::Status	Error(const elle::String&);

	//
	// attributes
	//
	elle::Gate*	gate;
      };

    }
  }
}

#endif
