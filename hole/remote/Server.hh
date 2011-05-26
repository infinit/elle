//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/remote/Server.hh
//
// created       julien quintard   [wed may 25 19:20:52 2011]
// updated       julien quintard   [thu may 26 14:42:25 2011]
//

#ifndef HOLE_REMOTE_SERVER_HH
#define HOLE_REMOTE_SERVER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <hole/remote/Node.hh>

namespace hole
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
      elle::Status	Push(const nucleus::Network&,
			     const nucleus::Address&,
			     const elle::Derivable<nucleus::Block>&);
      elle::Status	Pull(const nucleus::Network&,
			     const nucleus::Address&,
			     const nucleus::Version&);
      elle::Status	Wipe(const nucleus::Network&,
			     const nucleus::Address&);

      elle::Status	Connection(elle::Gate*&);
      elle::Status	Error(const elle::String&);

      //
      // attributes
      //
      elle::Gate*	gate;
    };

  }
}

#endif
