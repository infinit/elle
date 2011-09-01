//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/remote/Machine.hh
//
// created       julien quintard   [wed aug 31 00:52:52 2011]
// updated       julien quintard   [wed aug 31 15:05:31 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_REMOTE_MACHINE_HH
#define HOLE_IMPLEMENTATIONS_REMOTE_MACHINE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <hole/implementations/remote/Client.hh>
#include <hole/implementations/remote/Server.hh>

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
      /// XXX represents the current host
      ///
      class Machine:
	public elle::Entity
      {
      public:
	//
	// enumerations
	//
	enum Role
	  {
	    RoleUnknown,
	    RoleServer,
	    RoleClient
	  };

	//
	// constructors & destructors
	//
	Machine();
	~Machine();

	//
	// methods
	//
	elle::Status		Launch();

	//
	// interfaces
	//

	// dumpable
	elle::Status		Dump(const elle::Natural32 = 0) const;

	//
	// attributes
	//
	Role			role;

	union
	{
	  Server*		server;
	  Client*		client;
	};
      };

    }
  }
}

#endif
