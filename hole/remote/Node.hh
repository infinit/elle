//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/remote/Node.hh
//
// created       julien quintard   [thu may 26 09:46:18 2011]
// updated       julien quintard   [thu may 26 11:51:14 2011]
//

#ifndef HOLE_REMOTE_NODE_HH
#define HOLE_REMOTE_NODE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

namespace hole
{
  namespace remote
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a node being either a client or a server.
    ///
    class Node
    {
    public:
      //
      // constructors & destructors
      //
      Node(const nucleus::Network&,
	   const elle::Address&);

      //
      // interface
      //
      virtual elle::Status	Initialize() = 0;
      virtual elle::Status	Clean() = 0;

      virtual elle::Status	Put(const nucleus::Address&,
				    const nucleus::ImmutableBlock&) = 0;
      virtual elle::Status	Put(const nucleus::Address&,
				    const nucleus::MutableBlock&) = 0;
      virtual elle::Status	Get(const nucleus::Address&,
				    nucleus::ImmutableBlock&) = 0;
      virtual elle::Status	Get(const nucleus::Address&,
				    const nucleus::Version&,
				    nucleus::MutableBlock&) = 0;
      virtual elle::Status	Kill(const nucleus::Address&) = 0;

      //
      // attributes
      //
      nucleus::Network		network;
      elle::Address		host;
    };

  }
}

#endif
