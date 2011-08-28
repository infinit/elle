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
// updated       julien quintard   [sun aug 28 20:30:00 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_REMOTE_SERVER_HH
#define HOLE_IMPLEMENTATIONS_REMOTE_SERVER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <hole/implementations/remote/Peer.hh>
#include <hole/implementations/remote/Customer.hh>

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
	public Peer
      {
      public:
	//
	// types
	//
	typedef std::map<elle::Gate*, Customer*>	Container;
	typedef typename Container::iterator		Iterator;
	typedef typename Container::const_iterator	Scoutor;

	//
	// constructors & destructors
	//
	Server(const nucleus::Network&);
	~Server();

	//
	// methods
	//
	elle::Status		Add(elle::Gate*,
				    Customer*);
	elle::Status		Remove(elle::Gate*);
	elle::Status		Retrieve(elle::Gate*,
					 Customer*&);
	elle::Status		Locate(elle::Gate*,
				       Iterator* = NULL);

	//
	// interfaces
	//

	// peer
	elle::Status		Initialize(const elle::Point&);
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
	// callbacks
	//
	elle::Status		Connection(elle::Gate*&);
	elle::Status		Response(const elle::Cipher&);

	elle::Status		Authenticated();

	elle::Status		Push(const nucleus::Address&,
				     const
				       nucleus::Derivable<nucleus::Block>&);
	elle::Status		Pull(const nucleus::Address&,
				     const nucleus::Version&);
	elle::Status		Wipe(const nucleus::Address&);

	//
	// interfaces
	//

	// dumpable
	elle::Status		Dump(const elle::Natural32 = 0) const;

	//
	// attributes
	//
	Container		container;
      };

    }
  }
}

#endif
