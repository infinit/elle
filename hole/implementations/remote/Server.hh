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
// updated       julien quintard   [mon jul 18 22:55:11 2011]
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
	// enumerations
	//
	enum State
	  {
	    StateUnknown,
	    StateConnected,
	    StateAuthenticated
	  };

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
	elle::Status	Connection(elle::Gate*&);
	elle::Status	Response(const elle::Cipher&);

	elle::Status	Push(const nucleus::Address&,
			     const nucleus::Derivable<nucleus::Block>&);
	elle::Status	Pull(const nucleus::Address&,
			     const nucleus::Version&,
			     nucleus::Derivable<nucleus::Block>&);
	elle::Status	Wipe(const nucleus::Address&);

	elle::Status	Error(const elle::String&);

	//
	// interfaces
	//

	// dumpable
	elle::Status	Dump(const elle::Natural32 = 0) const;

	//
	// attributes
	//
	State		state;

	elle::Gate*	gate;
      };

    }
  }
}

#endif
