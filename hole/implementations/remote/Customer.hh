//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/implementations/remote/Customer.hh
//
// created       julien quintard   [sun aug 28 17:49:10 2011]
// updated       julien quintard   [sat sep  3 17:44:36 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_REMOTE_CUSTOMER_HH
#define HOLE_IMPLEMENTATIONS_REMOTE_CUSTOMER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

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
      class Customer:
	public elle::Entity
      {
      public:
	//
	// constants
	//
	static const elle::Natural32		Timeout;

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
	Customer();
	~Customer();

	//
	// methods
	//
	elle::Status		Create(elle::Gate*);

	//
	// callbacks
	//
	elle::Status		Disconnected();
	elle::Status		Error(const elle::String&);

	elle::Status		Abort();

	//
	// interfaces
	//

	// dumpable
	elle::Status		Dump(const elle::Natural32 = 0) const;

	//
	// attributes
	//
	State			state;

	elle::Gate*		gate;
	elle::Timer*		timer;

	//
	// signals
	//
	struct
	{
	  elle::Signal<
	    elle::Parameters<
  	      Customer*
	      >
	    >			dead;
	}			signal;
      };

    }
  }
}

#endif
